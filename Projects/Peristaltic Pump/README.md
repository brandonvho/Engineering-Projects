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

The Arduino sketch runs a non-blocking step loop using `AccelStepper` with `runSpeedToPosition()` so the keypad stays responsive during dispensing and the user can cancel mid-run with the `*` key.

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
| CAD | Autodesk Fusion 360 |
| Slicer / Printer | Bambu Lab P1S |
| IDE | Arduino IDE |

## Testing & Results
- **Session 1 (4/4/26):** Circuit schematic drafted in Tinkercad Circuits. First Arduino sketch written covering the full basic flow: keypad volume entry → stepper run → LCD status messages.
- **Session 2 (4/4/26):** Physical breadboard circuit assembled and first sketch uploaded. LCD, keypad, and 28BYJ-48 stepper all confirmed working. LCD bug (RW pin ungrounded) identified and fixed. Sketch significantly revised: blocking motor control replaced with non-blocking single-step-per-loop-tick approach, cancel via `*` key added, decimal input added, input validation added, LCD flicker fixed, and live progress percentage added.
- **Session 3 (4/23/26):** CAD work begun in Fusion 360. Designed the stepper motor base (mounting platform) and bearing rotor (rotating element that holds the bearings). Both parts sent to printer.
- **Session 4 (4/28/26):** Printed parts inspected against hardware. Multiple dimensional issues identified on both parts: axle hole undersized on motor base, base too thin causing screw protrusion into rotor sweep zone, bearing pegs undersized on rotor causing loose fit, and no tube clearance between bearing OD and outer wall. Revisions planned — rotor arm profile will also change from cross to zig-zag to introduce spring-like compliance for consistent tube pressure.
- **Session 5 (4/28/26):** Full sketch troubleshooting pass. Resolved all remaining software issues including broken cancel logic, LCD text overflow, motor reversal caused by the standard `Stepper` library's incompatibility with single-step non-blocking use, and AccelStepper configuration issues (wrong run method, missing acceleration, incorrect emergency stop). Motor now runs correctly at constant speed with working cancel and stable LCD.
- **Session 6 (4/28/26):** First liquid pump test. Electrical system confirmed fully functional. Pump did not draw water — bearing not compressing tube sufficiently, and bearing shifts laterally during rotation, breaking the peristaltic seal. Both issues are mechanical and will be addressed in the next rotor revision.

## Challenges & Lessons Learned
- **LCD showing nothing:** The RW (read/write) pin on the LCD1602 must be tied to GND for write-only operation. Leaving it floating causes the display to show nothing. Always check datasheet pin requirements before debugging in software.
- **Blocking motor code:** The initial `myStepper.step(N)` call locked the CPU for the entire dispense with no way to cancel. Refactored to a single-step-per-loop-tick approach so the keypad stays live and an emergency stop is possible.
- **Standard `Stepper` library incompatible with non-blocking use:** Calling `step(1)` repeatedly in a loop confused the library's internal timing, causing the 28BYJ-48 to stutter and reverse direction. Replaced with `AccelStepper`.
- **`AccelStepper` configuration is order-sensitive:** `runSpeed()` ignores `move()` targets; `run()` requires `setAcceleration()` to be defined or the motor won't move; `stop()` decelerates gradually rather than halting instantly. The correct pattern for this use case is `runSpeedToPosition()` with a fixed `setSpeed()`, which runs at constant speed with no ramp and respects `distanceToGo()`. Emergency stop must use `setCurrentPosition(currentPosition())` to immediately zero `distanceToGo()`.
- **ULN2003 pin order must match AccelStepper's expected coil sequence:** A mismatch causes the coils to fight each other — the motor shakes in place and draws high current. Diagnosed by swapping the D9 and D10 wires on the ULN2003 board. Verify pin order against the library's expected sequence whenever rewiring.
- **Bearing alignment is critical for peristaltic action:** A bearing that can drift laterally will not compress the tube perpendicularly, breaking the vacuum seal. Rotor design must constrain the bearing both radially and laterally throughout the full rotation.

## Future Improvements
- Fix rotor dimensional issues: increase bearing peg diameter, add tube clearance between bearing OD and outer wall, increase motor base thickness, and correct axle hole diameter.
- Redesign rotor arm profile from cross to zig-zag to introduce spring-like compliance and maintain consistent radial pressure on the tube throughout rotation.
- Add side walls to the rotor to constrain the bearing and tube laterally and keep them aligned.
- Physically calibrate `ML_PER_REV` by running a fixed number of revolutions and measuring displaced volume with a syringe or graduated cylinder.
- Design and print the outer pump housing with mounts for the screen and keypad.
- Replace the 28BYJ-48 with a NEMA 17 and an appropriate stepper driver (e.g., A4988 or DRV8825) for more torque and flow rate.
- Design and print all structural parts in PETG for better moisture and chemical resistance than PLA.
- Add a flow-rate mode so the user can specify mL/min in addition to total volume.
- Add decimal-unit selection (mL vs. fl oz) via the `B`/`C`/`D` keypad keys.