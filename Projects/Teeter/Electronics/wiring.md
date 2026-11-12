# Wiring - Teeter

Status: planned, not yet built. Diagram: `wiring_diagram.svg` (PNG copy alongside).

Everything is powered from the battery through one switch. The two drivers take battery voltage directly; the buck converter steps it down for the Arduino. The Arduino never powers the motors, it only sends step and direction signals.

## Connections

```
POWER (battery voltage, use thicker wire)
LiPo +  -> XT60 pigtail red   -> switch -> 12 V rail
LiPo -  -> XT60 pigtail black -> GND rail
12 V rail -> A4988 left VMOT, A4988 right VMOT, buck IN+
GND rail  -> A4988 left GND (next to VMOT), A4988 right GND, buck IN-
100 uF cap across VMOT and GND on each driver (stripe side to GND)
Buck OUT+ (7.5 V) -> Arduino Vin
Buck OUT-         -> Arduino GND

LOGIC (from the Arduino)
Arduino 5V  -> A4988 left VDD, A4988 right VDD, MPU-6050 VCC
Arduino GND -> A4988 left GND (next to VDD), A4988 right GND, MPU-6050 GND

SIGNALS
Arduino D3  -> A4988 left STEP
Arduino D4  -> A4988 left DIR
Arduino D5  -> A4988 right STEP
Arduino D6  -> A4988 right DIR
Arduino D7  -> EN on both A4988s
Arduino 20 (SDA) -> MPU-6050 SDA
Arduino 21 (SCL) -> MPU-6050 SCL

ON EACH A4988
RST jumpered to SLP
MS1 and MS2 -> 5V (1/8 microstepping)
MS3 left unconnected
1A, 1B -> one motor coil pair
2A, 2B -> the other coil pair
```

These pin numbers match `Arduino/code/code.ino`. STEP, DIR and EN pins can be changed in code; SDA and SCL on 20 and 21 are fixed on the Mega.

The EN wire lets the sketch release the motors when the robot falls past 45 degrees. EN is active-low: LOW = driver on.

## Notes

- **Buck converter set to 7.5 V into Vin, not 5 V into the 5V pin.** This allows USB to stay plugged in while the battery is on, which is needed for tuning.
- **Two GND pins per driver:** one next to VMOT (motor side), one next to VDD (logic side). Both get connected.
- **A4988 accepts 8-35 V on VMOT,** so the 3S LiPo connects directly.
- **Mount the IMU flat, rigid, and close to the wheel axle.** The sketch's accelerometer correction assumes this.

## Before powering anything

1. **Set the buck first.** Connect only the buck to the battery and adjust until a multimeter reads 7.5 V on the output (7.3-7.7 V is fine). They often ship set near battery voltage, which would overheat the Arduino's regulator.
2. **Set each driver's current limit** with its trim pot before attaching a motor (17HS4401 is rated 1.7 A; start around 1 A).
3. **Find the coil pairs.** Two motor wires belong to the same coil if the shaft gets harder to turn when they are touched together.
4. **Never plug or unplug a motor while the battery is on,** and double-check battery polarity: reversing it destroys the drivers instantly.

## Setting the LM2596 output

Do this with nothing connected to the output.

1. Battery + to IN+, battery - to IN-. Check polarity twice. Leave OUT+ and OUT- empty.
2. Multimeter on DC volts, 20 V range: red probe on OUT+, black on OUT-.
3. It will likely read close to battery voltage, around 10-12 V.
4. Turn the brass trim screw with a small flathead. Counterclockwise lowers the voltage on most modules.
5. Stop at 7.5 V.
6. Disconnect the battery, then wire OUT+ to Vin and OUT- to GND on the Arduino.

It is a multi-turn pot (about 25 turns end to end) and the reading often does not move for the first several turns. Recheck the voltage once the Arduino is connected.
