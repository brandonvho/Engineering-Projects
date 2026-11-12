"""
Self-balancing robot PID simulation - v5 (working)

Includes the real-world limits added in v4:

  * IMU model  - noisy angle reading, reported in degrees (as MPU6050
                 libraries do) and converted to radians before the PID
  * motor model - acceleration and top-speed limits for a stepper drive
  * disturbance - a shove at t = 2.5 s

Run it:  python balance_sim_v5.py   (needs matplotlib)
Tune KP / KI / KD below and re-run; the plot is saved as balance_sim_v5.png.

Model: inverted pendulum on a wheeled base, wheel acceleration as input.

    theta_ddot = (g*sin(theta) - a*cos(theta)) / L

theta = tilt from vertical (rad, + = leaning forward)
a     = wheel/base acceleration (m/s^2, + = forward)
"""
import math
import random
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

# --- physical parameters ---
G = 9.81          # m/s^2
L = 0.10          # m, wheel axle to centre of mass

# --- motor limits (stepper + ~80 mm wheels, rough estimates) ---
MAX_ACCEL = 8.0  # m/s^2
MAX_SPEED = 0.8   # m/s

# --- IMU model ---
IMU_NOISE_DEG = 0.1   # std dev of angle reading, degrees

# --- timing ---
DT_CTRL = 1 / 200     # control loop period (200 Hz)
DT_PHYS = 1 / 1000    # physics step
T_END = 6.0

# --- PID gains (output = wheel acceleration, m/s^2 per rad) ---
KP = 25
KI = 0
KD = 1.5

SETPOINT = 0.0                    # upright
THETA0 = math.radians(5.0)        # start tilted 5 degrees

# --- disturbance: a shove that adds tilt rate ---
PUSH_TIME = 2.5       # s
PUSH_KICK = 0.5       # rad/s

SEED = 1


def read_imu(theta, rng):
    """Tilt angle as the IMU library reports it: degrees, with noise."""
    return math.degrees(theta) + rng.gauss(0.0, IMU_NOISE_DEG)


def run():
    rng = random.Random(SEED)
    theta, omega = THETA0, 0.0    # tilt, tilt rate
    x, v = 0.0, 0.0               # base position, velocity
    integral, prev_error = 0.0, None
    accel = 0.0
    t, next_ctrl = 0.0, 0.0
    pushed = False
    log = {"t": [], "theta": [], "v": [], "a": []}
    fell_at = None

    while t < T_END:
        if t >= next_ctrl:
            # v5 fix: the IMU reports degrees but the gains are per radian.
            angle = math.radians(read_imu(theta, rng))
            error = angle - SETPOINT
            integral += error * DT_CTRL
            # no derivative kick on the very first sample
            derivative = 0.0 if prev_error is None else (error - prev_error) / DT_CTRL
            prev_error = error
            accel = KP * error + KI * integral + KD * derivative
            accel = max(-MAX_ACCEL, min(MAX_ACCEL, accel))
            next_ctrl += DT_CTRL

        # motor cannot accelerate past its top speed
        a = accel
        if (v >= MAX_SPEED and a > 0) or (v <= -MAX_SPEED and a < 0):
            a = 0.0

        if not pushed and t >= PUSH_TIME:
            omega += PUSH_KICK
            pushed = True

        alpha = (G * math.sin(theta) - a * math.cos(theta)) / L
        omega += alpha * DT_PHYS
        theta += omega * DT_PHYS
        v += a * DT_PHYS
        x += v * DT_PHYS
        t += DT_PHYS

        log["t"].append(t)
        log["theta"].append(math.degrees(theta))
        log["v"].append(v)
        log["a"].append(a)

        if abs(theta) > math.radians(45):
            fell_at = t
            break

    return log, fell_at


if __name__ == "__main__":
    log, fell_at = run()
    if fell_at is not None:
        print(f"FELL OVER at t = {fell_at:.3f} s")
    else:
        print(f"Still upright at t = {T_END} s")
    print(f"max |tilt| = {max(abs(a) for a in log['theta']):.2f} deg")
    print(f"final tilt = {log['theta'][-1]:.2f} deg, final speed = {log['v'][-1]:.2f} m/s")

    fig, ax = plt.subplots(3, 1, figsize=(8, 8), sharex=True)
    ax[0].plot(log["t"], log["theta"]); ax[0].set_ylabel("tilt (deg)"); ax[0].grid(True)
    ax[1].plot(log["t"], log["v"]); ax[1].set_ylabel("wheel speed (m/s)"); ax[1].grid(True)
    ax[2].plot(log["t"], log["a"]); ax[2].set_ylabel("wheel accel (m/s^2)")
    ax[2].set_xlabel("time (s)"); ax[2].grid(True)
    fig.suptitle("Balance sim v5")
    import os                                                  # file and folder tools
    script_path = os.path.abspath(__file__)                    # full path of this .py file
    png_path = os.path.splitext(script_path)[0] + ".png"       # same folder, same name, .png ending
    fig.savefig(png_path, dpi=110)
    print(f"graph saved to {png_path}")
