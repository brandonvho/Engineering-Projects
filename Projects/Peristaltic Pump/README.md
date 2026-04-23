# Peristaltic Pump

## Overview
Building a peristaltic pump using an Arduino Mega, a stepper motor, a 4×4 keypad, a 16×2 LCD, and a 3D-printed housing. The pump draws liquid through flexible silicone tubing by compressing it with rotating ball bearings, creating a vacuum that moves fluid without it ever contacting the motor or electronics.

## Objectives
- Learn practical circuit design and breadboarding.
- Learn how to program an Arduino and interface common peripherals (LCD, keypad, stepper motor).
- Build a working peristaltic pump that transfers a user-specified volume of liquid between containers.
- Calibrate stepper motor steps to accurately dispense a target volume in mL.

## Requirements & Constraints
- Primary hardware must come from the Elegoo Mega Most Complete Starter Kit, supplemented by a few Amazon purchases (silicone tubing, bearings, 12 V power supply).
- Pump body, rotor, and roller arm must be 3D printed.
- The user interface must allow volume entry via keypad and display status on an LCD — no computer connection needed during operation.

## Design Summary
The 28BYJ-48 stepper motor (driven by its ULN2003 board) rotates a 3D-printed rotor fitted with ball bearings. Those bearings compress a loop of silicone tubing against a curved housing wall, pushing fluid through the tube one "pinch" at a time. Volume is entered on a 4×4 membrane keypad; the LCD1602 shows the prompt, live dispensing progress, and a completion message. A 12 V external power supply (routed through the kit's power supply module) will drive the larger replacement stepper motor in later iterations.

The Arduino sketch runs a non-blocking step loop so the keypad stays responsive during dispensing and the user can cancel mid-run with the `*` key.

## Tools & Technologies
| Category | Item |
|---|---|
| Microcontroller | Elegoo Mega 2560 |
| Motor driver | ULN2003 (included in kit) |
| Stepper (prototype) | 28BYJ-48 (included in kit) |
| Stepper (target) | NEMA 17 (to be wired with external driver) |
| Display | LCD1602 Module (included in kit) |
| Input | 4×4 Membrane Keypad (included in kit) |
| Power | Power Supply Module + 12 V DC adapter |
| CAD | Autodesk Fusion |
| Slicer / Printer | Bambu Lab P1S |
| IDE | Arduino IDE |

## Testing & Results
- **Session 1 (4/4/26):** Circuit schematic drafted. Chose Tinkercad Circuits for quick virtual layout before moving to physical breadboard.
- **Session 2 (4/4/26):** Physical breadboard circuit assembled and first sketch uploaded. LCD, keypad, and 28BYJ-48 stepper all confirmed working. LCD bug (RW pin ungrounded) identified and fixed. The pump correctly prompts for volume, runs the motor, and displays a completion message.

## Challenges & Lessons Learned
- **LCD showing nothing:** The RW (read/write) pin on the LCD1602 must be tied to GND for write-only operation. Leaving it floating causes the display to show nothing. Always check datasheet pin requirements before debugging in software.
- **Blocking motor code:** The initial `myStepper.step(N)` call locked the CPU for the entire dispense with no way to cancel. Refactored to a single-step-per-loop-tick approach so the keypad stays live and an emergency stop is possible.

## Future Improvements
- Replace the 28BYJ-48 with a NEMA 17 and an appropriate stepper driver (e.g., A4988 or DRV8825) for significantly more torque and flow rate.
- Physically calibrate `ML_PER_REV` by running a known number of revolutions and measuring displaced volume with a syringe or graduated cylinder.
- Design and print the pump body, rotor, and roller arms in PETG (better moisture and chemical resistance than PLA).
- Add a flow-rate mode so the user can specify mL/min in addition to total volume.
- Add decimal-unit selection (mL vs. fl oz) via the `B`/`C`/`D` keypad keys.