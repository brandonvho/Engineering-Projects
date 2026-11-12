# PID math notes

## The equation

A PID controller computes an output from the error between the target and the measurement:

    e(t) = r(t) - y(t)

    u(t) = Kp*e(t) + Ki * integral of e dt + Kd * de/dt

- r = setpoint. On Teeter: upright, 0 degrees
- y = measurement. On Teeter: tilt angle from the IMU
- u = actuator command. On Teeter: wheel acceleration

Note: on Teeter the error is taken as `tilt - setpoint`, the opposite sign to the textbook form, because a forward lean needs a forward wheel acceleration (sim v1 bug).

## The three terms

- **Proportional (present):** pushes back in proportion to the error, like a spring. Too high and the system oscillates. Alone, it usually leaves a steady-state offset.
- **Integral (past):** accumulates error over time, so a small persistent offset eventually builds enough output to remove it. Too high gives overshoot and slow oscillation.
- **Derivative (future):** reacts to how fast the error is changing, like a damper. Reduces overshoot but amplifies sensor noise.

## Frequency domain

Controller transfer function:

    C(s) = Kp + Ki/s + Kd*s

With plant G(s), the closed loop is:

    Y/R = C(s)G(s) / (1 + C(s)G(s))

Tuning the gains moves the closed-loop poles. An inverted pendulum has a pole in the right half-plane (unstable). The controller's job is to pull it into the left half-plane.

## Discrete form (what runs on a microcontroller)

With sample time dt:

    error      = setpoint - angle
    integral  += error * dt
    derivative = (error - prev_error) / dt
    output     = Kp*error + Ki*integral + Kd*derivative
    prev_error = error

## Three practical fixes

- **Anti-windup:** clamp the integral so it does not grow while the motors are saturated.
- **Derivative on measurement:** differentiate the measurement instead of the error, to avoid spikes when the setpoint changes. On the robot, the gyro rate gives this directly.
- **Filtering:** low-pass the D term, since differentiation amplifies noise.

## Plant model used in the sim

    theta_ddot = (g*sin(theta) - a*cos(theta)) / L

- theta = tilt from vertical (rad, + = leaning forward)
- a = wheel acceleration (m/s², + = forward)
- L = wheel axle to centre of mass (m)
