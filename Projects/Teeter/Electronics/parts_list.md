# Parts list - Teeter

## Already on hand

| Item | Notes |
|---|---|
| ELEGOO Mega 2560 R3 ("Most Complete Starter Kit") | Controller. Kit also supplies jumper wires and a breadboard |
| NEMA 17 stepper (1) | First wheel motor |
| A4988 stepper driver (1) | From the earlier peristaltic pump project |
| 3D printer | Chassis, motor mounts, wheels |

## Purchased (Amazon, Nov 2026)

| Item | Purpose | Search term used |
|---|---|---|
| NEMA 17 stepper (1 more) | Second wheel motor, matching the first | `NEMA 17 stepper motor 17HS4401` |
| A4988 driver (1 more) | Each motor needs its own driver to turn step signals into coil current | `A4988 stepper driver module` |
| MPU-6050 IMU (GY-521) | Gyro and accelerometer that measure tilt angle, the input to the PID | `MPU-6050 GY-521 module` |
| 3S LiPo, 11.1 V, XT60 | Powers the motors directly and makes the robot untethered | `3S LiPo 11.1V 1500mAh XT60` |
| LiPo balance charger | Charges the battery safely, cell by cell | `LiPo balance charger 3S` |
| LM2596 buck converter | Steps the battery down for the Arduino (set to 7.5 V into Vin) | `LM2596 buck converter module` |
| 100 µF 35 V capacitors (2) | Absorb voltage spikes at each driver's motor power pins | `100uF 35V electrolytic capacitor` |
| XT60 connector pigtail | Matches the battery plug | `XT60 connector pigtail` |
| Rocker switch | Cuts battery power without unplugging | `rocker switch 12V` |
| M3 screws and nuts | Mount the motors and hold the printed chassis together | `M3 screw nut assortment` |

## Not purchased

| Item | Purpose | Status |
|---|---|---|
| LiPo low-voltage alarm | Beeps before the battery over-discharges (stop at about 3.3 V per cell) | Skipped for now |
| Multimeter | Needed to set the buck converter output and the A4988 current limits | Confirm one is available before first power-up |
| Arduino Nano | Smaller and lighter than the Mega | Optional, not needed |
| ESP32 | Wireless gain tuning | Possible later upgrade |
| TPU filament or O-rings | Tire grip | Optional |

## Decisions

- **Motors: NEMA 17 steppers, not DC gear motors.** No gearbox means no backlash, and speed is exactly what is commanded, so no encoders are needed and tuning is cleaner. Cost: heavier, and they draw current standing still.
- **Battery: 3S LiPo, not AA cells.** Two NEMA 17s pull roughly 1-2 A continuously; alkaline AAs sag badly at that current and eight would be needed to reach 12 V.
- **Controller: stay on the Mega.** The loop only needs 100-200 Hz. An ESP32 is the upgrade if tethered tuning gets annoying.
- **Step pulses from a timer interrupt, not AccelStepper,** which is likely too slow on the Mega for this.

## LiPo care

- Charge only with the balance charger, and stay nearby while it charges.
- Stop using it at about 3.3 V per cell.
- Do not puncture or crush it; stop using it if it puffs up.
