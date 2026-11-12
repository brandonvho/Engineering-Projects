# Balance sim: iteration log

Simulation of a two-wheel self-balancing robot (inverted pendulum on a wheeled base) with a PID loop on tilt angle. I went through five versions; each entry records what the version did when I ran it, how I found the cause, and the fix.

All numbers below come from running the scripts as saved (`python balance_sim_vN.py`). Start condition in every version: 5° forward tilt, at rest.

| Version | Change | Result |
|---|---|---|
| v1 | First attempt | Falls over in 0.13 s |
| v2 | Error sign fixed | Oscillation grows until it falls at 5.2 s |
| v3 | Derivative divided by dt | Balances, but with a perfect sensor and motor |
| v4 | Sensor noise, motor limits, shove added | Upright, but motor command pinned at its limit |
| v5 | IMU degrees converted to radians | Balances with a realistic command |

---

## v1: first attempt

**Result:** fell past 45° at t = 0.132 s.

**Finding the cause:** 0.13 s is suspiciously fast, so I ran the same script with all gains set to zero. With no controller at all it takes 0.293 s to fall. The controller was making it fall more than twice as fast, which means positive feedback rather than weak gains.

Checking the first sample confirmed it: at +5° tilt (leaning forward) the commanded wheel acceleration was −3.70 m/s² (backward). The wheels were driving out from under the robot.

**Cause:** `error = SETPOINT - theta`. I used the textbook form, but for this plant a forward lean needs a forward wheel acceleration, so the error has to be `theta - SETPOINT`.

**Fix (v2):** flip the sign of the error.

---

## v2: sign fixed

**Result:** no longer falls immediately, but oscillates with growing amplitude. Tilt peaks: 5.83° at 0.36 s, 6.80°, 7.94°, 9.26° ... 40.18° at 4.83 s. Still technically upright when the 5 s run ends; extending the run to 10 s shows it falling at 5.22 s.

**Finding the cause:** a steady oscillation that grows about 17% per cycle means the loop has stiffness but no damping. Damping is the D term's job, so I checked its size: for a tilt rate of 1 rad/s the D term contributed 0.012 m/s², where Kd = 2.4 should give 2.4 m/s². It was 200 times too small, and 200 is the loop rate in Hz.

**Cause:** I wrote `derivative = error - prev_error`, which is the change per sample, not per second.

**Fix (v3):** `derivative = (error - prev_error) / DT_CTRL`.

---

## v3: derivative fixed

**Result:** balances. Within 0.5° of upright by t = 0.21 s, one undershoot to −1.20°, final tilt 0.00°.

**Problem found on inspection:** the peak wheel acceleration was 45.4 m/s², on the very first control sample. Two things wrong with that:

1. It is a derivative kick. `prev_error` starts at 0, so the first derivative sees the 5° initial tilt as an instant jump.
2. A stepper driving a small robot cannot deliver anything close to 45 m/s². The sim only works because the motor and sensor are ideal.

So v3 passes, but it would not predict anything useful about the real robot.

**Change (v4):** I added an IMU model (0.1° noise, reading in degrees like MPU6050 libraries report), motor limits (15 m/s² acceleration, 1.5 m/s top speed), a shove at t = 2.5 s (0.5 rad/s added to tilt rate), and skipped the derivative on the first sample.

---

## v4: sensor noise, motor limits, disturbance

**Result:** stays upright for the full 6 s, so the pass/fail printout looks fine. The acceleration plot does not: it is a solid block between +15 and −15 m/s².

- Command at the acceleration limit 90% of the time
- Command reverses direction on 700 of 1200 control steps
- RMS wheel acceleration between 1.0 and 2.4 s (nothing happening): 14.4 m/s²
- RMS tilt over the same window: 0.45°

It is behaving as a bang-bang controller, slamming the motor between full forward and full reverse. A real stepper would skip steps or stall, and the robot would shake itself apart.

**Finding the cause:** saturation that constant means the loop gain is far too high, but the gains are the same ones that worked in v3. The only new thing between the plant and the PID is the IMU model. For a 5° tilt the P term came out as 200 m/s² where 3.49 was expected: a ratio of 57.3, which is degrees per radian.

**Cause:** the IMU reading is in degrees and I passed it straight into gains tuned per radian.

**Fix (v5):** `angle = math.radians(read_imu(theta, rng))`.

---

## v5: units fixed (working version)

**Result:** balances with a command the motor can actually follow.

| Metric | v4 | v5 |
|---|---|---|
| Time at acceleration limit | 90% | 0% |
| RMS wheel acceleration, 1.0 to 2.4 s | 14.4 m/s² | 1.22 m/s² |
| RMS tilt, 1.0 to 2.4 s | 0.45° | 0.07° |
| Peak tilt after the shove | 1.38° | 0.80° |

Recovery from the shove: back within 0.5° at t = 2.62 s, 0.12 s after the push.

**Extra checks I ran:**
- Five different noise seeds (1 to 5): upright in all of them
- Larger shoves of 1, 2, 3 and 4 rad/s: upright in all of them

### Known limitations

1. **Noisy command.** The wheel acceleration still reverses direction on 775 of 1200 control steps, because the D term differentiates the noisy angle. Amplitude is small (1.22 m/s² RMS) but a low-pass filter on the derivative, or using the gyro rate directly, would clean it up.
2. **Drift.** After the shove the robot keeps rolling at about 0.09 m/s (0.08 to 0.09 across seeds). The controller only looks at tilt, so nothing brings wheel speed back to zero. An outer velocity loop would fix this.
3. **Guessed parameters.** `L`, the motor limits and the IMU noise are my estimates. They need replacing with measurements from the real robot before the gains mean much.
4. **Simplified physics.** Point-mass pendulum, no friction, no step quantisation, no sensor delay.
