# Engineering Log - Teeter

---

## 11/11/26 — Session 1: Project selection and parts

**Work completed**
- Compared three "balancing PID" builds: ball on beam, two-wheel self-balancing robot, ball on plate. I chose the two-wheel robot
- Chose NEMA 17 steppers over DC gear motors with encoders (no backlash, no encoder code)
- Chose a 3S LiPo over AA cells (two steppers draw roughly 1-2A continuously)
- Decided to stay on the Mega 2560 for now; ESP32 noted as a later upgrade for wireless tuning
- Built the parts list and ordered everything on it from Amazon except the LiPo low-voltage alarm
- Named the project Teeter

**Issues**
- On my earlier peristaltic pump project I could not get more than 9 V to the motor from the Arduino. Cause: the Arduino should never power motors; Vin only passes through the barrel-jack supply. Here the battery feeds the drivers directly

**Notes**
- Planned layout: motors at the bottom, electronics in the middle, battery at the top (a higher centre of mass falls more slowly).
- AccelStepper is likely too slow on the Mega for this; step pulses will come from a timer interrupt
- Parts list: `Electronics/parts_list.md`

**Next**
- Understand the PID math
- Simulate before building

---

## 11/11/26 — Session 2: PID math and Python simulation

**Work completed**
- Worked through the PID equation, its discrete form, and the practical fixes (anti-windup, derivative on measurement, filtering). Notes in `Notes/pid_math.md`
- Wrote a Python simulation of the robot as an inverted pendulum on a wheeled base, and debugged it over five versions (`Simulation/`)
- v5 balances from a 5 degree start and recovers from a shove in 0.12 s with Kp = 40, Ki = 5, Kd = 2.4

**Issues**
- v1: I had the error sign backwards. Fell in 0.13 s, faster than with no controller (0.29 s)
- v2: I forgot to divide the derivative by dt. D term 200 times too small, oscillation grew about 17% per cycle
- v3: balanced, but only with an ideal sensor and a 45 m/s² peak motor command
- v4: I fed the IMU angle in degrees into gains tuned per radian. Stayed upright, but the motor command was at its limit 90% of the time
- Full detail for each: `Simulation/ITERATIONS.md`

**Notes**
- v4 never fell over; the failure only showed in the acceleration plot. Check the command, not just pass/fail
- v5 still drifts at about 0.09 m/s after a shove because the controller only looks at tilt
- Centre-of-mass height (10 cm), motor limits, and IMU noise in the sim are estimates

**Next**
- Port the controller to Arduino
- Replace the estimates with measurements once the robot exists

---

## 11/11/26 — Session 3: Arduino sketch, wiring plan, learning plan

**Work completed**
- Ported my v5 controller to an Arduino sketch for the Mega, two A4988s, and the MPU-6050 (`Arduino/code/code.ino`)
- Tested the sketch on a PC against a simulated robot: it balances and recovers from shoves
- Drew the wiring diagram and wrote out every connection (`Electronics/wiring.md`, `Electronics/wiring_diagram.svg`)
- Wrote down the procedure for setting the LM2596 output

**Issues**
- My direct port of v5 fell over in the PC test. I had to add two things that the Python sim's ideal sensor had hidden:
  - Accelerometer correction: the accelerometer reads "leaning back" whenever the wheels accelerate forward
  - Speed loop (KV): lean back as wheel speed rises, to stop the robot running away
- I have not yet compiled the sketch in the Arduino IDE or run it on hardware

**Notes**
- Buck converter set to 7.5 V into Vin (not 5 V into the 5V pin) so USB can stay connected while the battery is on
- Mount the IMU close to the wheel axle: in the PC test 5 cm above the axle was fine, 10 cm was noticeably rougher
- A multimeter is needed for the buck output and the driver current limits
- The sketch is a first draft that has never touched hardware. I plan to rebuild it in small pieces, each tested on the real parts. Plan in `Notes/learning_plan.md`

**Next**
- When parts arrive: set the buck to 7.5 V, set driver current limits, read the MPU-6050
- Start the chassis CAD in Fusion 360 (motor mounts, wheels, shelves)

---

## MM/DD/YY — Session 4: [Short session title]

**Work completed**
-

**Issues**
-

**Notes**
-

**Next**
-

---
