"""
Self-balancing robot PID simulation - v1 (first attempt)

Model: inverted pendulum on a wheeled base. Stepper motors are speed
controlled, so the controller output is treated as wheel acceleration.

    theta_ddot = (g*sin(theta) - a*cos(theta)) / L

theta = tilt from vertical (rad, + = leaning forward)
a     = wheel/base acceleration (m/s^2, + = forward)
"""
import math
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

# --- physical parameters ---
G = 9.81          # m/s^2
L = 0.10          # m, wheel axle to centre of mass

# --- timing ---
DT_CTRL = 1 / 200     # control loop period (200 Hz)
DT_PHYS = 1 / 1000    # physics step
T_END = 5.0

# --- PID gains (output = wheel acceleration, m/s^2 per rad) ---
KP = 40.0
KI = 5.0
KD = 2.4

SETPOINT = 0.0                    # upright
THETA0 = math.radians(5.0)        # start tilted 5 degrees


def run():
    theta, omega = THETA0, 0.0    # tilt, tilt rate
    x, v = 0.0, 0.0               # base position, velocity
    integral, prev_error = 0.0, 0.0
    accel = 0.0
    t, next_ctrl = 0.0, 0.0
    log = {"t": [], "theta": [], "v": [], "a": []}
    fell_at = None

    while t < T_END:
        if t >= next_ctrl:
            error = SETPOINT - theta
            integral += error * DT_CTRL
            derivative = error - prev_error
            prev_error = error
            accel = KP * error + KI * integral + KD * derivative
            next_ctrl += DT_CTRL

        alpha = (G * math.sin(theta) - accel * math.cos(theta)) / L
        omega += alpha * DT_PHYS
        theta += omega * DT_PHYS
        v += accel * DT_PHYS
        x += v * DT_PHYS
        t += DT_PHYS

        log["t"].append(t)
        log["theta"].append(math.degrees(theta))
        log["v"].append(v)
        log["a"].append(accel)

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

    fig, ax = plt.subplots(2, 1, figsize=(8, 6), sharex=True)
    ax[0].plot(log["t"], log["theta"]); ax[0].set_ylabel("tilt (deg)"); ax[0].grid(True)
    ax[1].plot(log["t"], log["v"]); ax[1].set_ylabel("wheel speed (m/s)")
    ax[1].set_xlabel("time (s)"); ax[1].grid(True)
    fig.suptitle("Balance sim v1")
    fig.savefig("balance_sim_v1.png", dpi=110)
