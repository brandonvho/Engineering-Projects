# Design Log – Peristaltic Pump

---

## 04/04/26 — Session 1: Schematic & Initial Sketch

**Work completed**
- Wrote the first Arduino sketch covering the full basic flow: keypad volume entry → stepper run → LCD status messages.
- Explored circuit design software options; settled on Tinkercad Circuits because it includes the Arduino Mega and common kit components (LCD, keypad, ULN2003/28BYJ-48) in its library and runs in the browser with no install.

**Issues**
- Unsure which schematic tool supported the exact Elegoo kit parts. Tinkercad was chosen as the quickest path to a working virtual layout.

**Notes**
- `ML_PER_REV = 1.2` is a placeholder — actual value must be measured once the pump body is printed and tubing is installed.
- `STEPS_PER_REV = 2048` is correct for the 28BYJ-48 in half-step mode via the ULN2003.

**Next**
- Assemble the circuit on a physical breadboard and upload the sketch to confirm all components work together.

---

## 04/21/26 — Session 2: Breadboard Build & Sketch Revision

**Work completed**
- Wired the breadboard circuit: 28BYJ-48 + ULN2003, LCD1602, and 4×4 membrane keypad connected to the Mega.
- Uploaded the sketch and confirmed the LCD prompts for volume, the keypad registers input, the stepper runs, and the LCD shows "Dispensing…" and "Done!" messages.
- Revised the sketch significantly after identifying several issues (see below).

**Issues**
- **LCD blank on startup:** The RW (read/write) pin was left unconnected. Tying it to GND (write-only mode) fixed the display immediately.
- **No way to cancel a running dispense:** The original `myStepper.step(stepsToRun)` call is blocking — the CPU does nothing else until all steps complete. If the motor needs to be stopped early, the only option was a hard reset.
- **No decimal input:** Volume could only be entered as whole numbers; entering 2.5 mL was impossible.
- **No input validation:** Typing a very large number would queue an extremely long motor run with no warning.
- **LCD flicker:** Clearing a line by printing 16 spaces caused visible flicker on every keypress.

**Sketch changes made**
| Area | Before | After |
|---|---|---|
| Motor control | `myStepper.step(N)` — blocks CPU until done | `myStepper.step(1)` called once per loop tick (non-blocking) |
| Cancel | Not possible mid-run | `*` key sets `stepsRemaining = 0`, stopping the motor immediately |
| Decimal input | Not supported | `A` key inserts a `.`; only one decimal allowed; leading `0` added automatically |
| Input guard | None | Maximum 5 characters; maximum 999.9 mL; rejects 0 or negative values |
| LCD update | 16-space overwrite (causes flicker) | Padded fixed-width string written in one `lcd.print` call |
| Progress display | Static "Dispensing…" for entire run | Live percentage updated every 64 steps |
| State machine | Implicit (single loop path) | Explicit `ENTERING` / `DISPENSING` enum; keypad always checked |

**Notes**
- Units are not yet displayed on the LCD — the sketch assumes mL but does not label it. Will add once unit selection is implemented.
- Steps-per-mL has not been calibrated; the pump body does not exist yet, so this will be done once printing and tube installation are complete.
- The 28BYJ-48 at 10–12 RPM is slow but sufficient for functional testing. Torque may be insufficient for stiffer tubing — this will inform whether the NEMA 17 upgrade is necessary.

**Next**
- Design the pump body, rotor, and roller arm in Fusion.
- Calculate theoretical volume per revolution from tube inner diameter and rotor geometry; compare to measured value after printing.
- Calibrate `ML_PER_REV` empirically: run a fixed number of revolutions, measure displaced volume, divide.

---

## 04/23/26 — Session 3: CAD – Motor Base & Bearing Rotor

**Work completed**
- Designed the stepper motor base in Fusion 360 — a mounting platform that the stepper bolts to, forming the structural base of the full pump assembly.
- Designed the bearing rotor in Fusion 360 — the rotating element that holds the bearings which will squeeze the tube against the pump body wall to create peristaltic action.
- Sent both parts to the printer.

**Issues**
- None identified at this stage — fit and tolerance will be assessed once the print is complete.

**Notes**
- Both parts are currently printing; dimensional accuracy of the bearing rotor (especially bearing pocket depth and diameter) is critical — bearings must seat firmly without play.
- The rotor-to-motor shaft interface should be checked for fit before assembling; reprint with adjusted tolerance if the shaft is loose or won't seat.

**Next**
- Inspect prints for dimensional accuracy and layer quality.
- Test-fit the rotor onto the motor shaft and confirm bearing seats hold bearings securely.
- Assemble the base and rotor and evaluate clearances before designing the pump body outer shell.
- Begin tubing path design once rotor geometry is confirmed.

---

## 04/28/26 — Session 4: Print Inspection & Revision Planning

**Work completed**
- Inspected both printed parts against the stepper motor and bearings.

**Issues**
- **Motor base — axle hole undersized:** The hole for the stepper motor axle is too small; the motor does not sit flush against the base, causing the axis of rotation to be non-perpendicular.
- **Motor base — insufficient thickness:** The base is too thin; the M3 mounting screws protrude far enough to contact the bearing rotor during rotation.
- **Motor base — M3 screw holes:** Hole sizing for the M3 mounting screws is correct — they self-tap without needing threaded inserts.
- **Bearing rotor — peg diameter undersized:** The pegs that retain the bearings are too small; bearings seat loosely and fall off easily.
- **Bearing rotor — no tube clearance:** There is no gap between the bearing and the outer wall, leaving no space for the silicone tube to pass through.

**Notes**
- All identified issues are dimensional — small sizing and fitment adjustments in Fusion, no geometry rethink required for the base.
- The rotor will undergo a more significant design change: the cross-shaped arm profile will be replaced with a zig-zag (spring) profile so each arm flexes slightly, maintaining constant radial pressure on the tube. This ensures the bearing stays in contact with the tube throughout rotation, preserving vacuum and consistent flow.
- Tube clearance will be added between the bearing OD and the outer wall as part of the same rotor revision.

**Next**
- Increase axle hole diameter on the motor base so the motor seats flush and the shaft is perpendicular.
- Increase base thickness so M3 screws do not reach the rotor sweep zone.
- Increase bearing peg diameter on the rotor so bearings are held securely.
- Add radial clearance between bearing OD and outer wall to accommodate the silicone tube.
- Redesign rotor arm profile from cross to zig-zag to introduce spring-like compliance and maintain tube pressure.
- Reprint both parts and re-inspect.

---

## 04/28/26 — Session 5: Sketch Troubleshooting

**Work completed**
- Conducted a full review of the sketch and resolved all identified issues from initial review through hardware testing. Motor is now running correctly at constant speed with working cancel, progress display, and stable LCD.

**Issues**
- **Initial sketch review — multiple issues before hardware testing:** The first sketch had a blocking `step(N)` call that locked the CPU for the entire dispense with no cancel path, no decimal input, no input length or volume validation, 16-space LCD line clearing causing flicker, and no progress feedback during dispensing. All were rewritten into the first improved version.
- **`*` cancel not working during dispense:** The keypad check was placed after the `if (state == DISPENSING)` return, so it never executed while the motor was moving. Fixed by moving the keypad read inside the dispensing branch, before `runOneStep()`.
- **LCD text overflow — "mL" getting cut off:** The string `"Dispensing " + volume + "mL"` exceeded the 16-character display width. Fixed by shortening line 0 to just the volume (e.g. `"5.0 mL"`) and placing the cancel hint on line 1 (`"Running [*]=stop"`).
- **Motor reversing mid-run:** The standard Arduino `Stepper` library is not designed for single-step non-blocking use. Calling `step(1)` repeatedly in a loop confused its internal timing and caused the 28BYJ-48 to stutter and reverse. Fixed by replacing the `Stepper` library with `AccelStepper`, which handles coil sequencing and timing correctly for this pattern.
- **Motor not moving, progress stuck at 0%:** After switching to `AccelStepper`, `runSpeed()` was used instead of `run()`. `runSpeed()` ignores the target set by `move()` and runs indefinitely without checking `distanceToGo()`, so the progress counter never incremented. Fixed by replacing `runSpeed()` with `run()`.
- **Motor trying but not moving:** `AccelStepper.run()` uses an acceleration ramp; without `setAcceleration()` defined it defaulted to zero, so the motor never ramped up to a usable speed. Fixed by adding `myStepper.setAcceleration(200)` in `setup()`.
- **Screen going blank:** The stepper motor drawing current from the Arduino's 5V pin was browning out the board and killing the LCD. Fixed by powering the ULN2003 driver board from the kit's external power supply module instead of the Arduino's 5V rail.
- **`*` emergency stop not working after `AccelStepper` switch:** `myStepper.stop()` decelerates gradually rather than halting instantly, so `distanceToGo()` remained non-zero and the done/cancelled branch in `runOneStep()` never triggered. Fixed by replacing `myStepper.stop()` with `myStepper.setCurrentPosition(myStepper.currentPosition())`, which immediately zeros `distanceToGo()`.
- **Motor shaking and fighting itself:** The coils were firing in the wrong sequence, causing them to oppose each other. Root cause was a mismatch between `AccelStepper`'s expected pin order and the physical ULN2003 wiring. Fixed by physically swapping the D9 and D10 wires on the ULN2003 board rather than changing the code.
- **Unwanted acceleration and deceleration:** `AccelStepper.run()` acceleration ramp felt unnatural for a pump application. Fixed by switching to `runSpeedToPosition()` with a fixed `setSpeed(400)` called after `move()`, which runs at constant speed with no ramp and ignores `setAcceleration()` entirely.
- **Typo compile error — `myStpper` not declared:** A typo in the `setSpeed` call caused a compile error. Fixed by correcting the spelling to `myStepper`.
- **RW pin — LCD blank on first breadboard build:** The RW (read/write) pin on the LCD1602 was left unconnected. It must be tied to GND for write-only operation — leaving it floating prevents any display output regardless of all other wiring being correct.

**Notes**
- `AccelStepper` with `runSpeedToPosition()` and `setSpeed()` is now the correct pattern for this use case — constant speed, no ramp, respects `distanceToGo()`.
- Power separation between the Arduino 5V rail and the ULN2003 driver is essential; any future motor upgrades should be treated the same way.
- Pin order on the ULN2003 must be verified against `AccelStepper`'s expected sequence whenever rewiring — coil fight is silent and hard to diagnose.
- Speed was set to `setSpeed(350)` — determined to be the fastest speed at which the motor retains enough torque to run without stalling.

**Next**
- Calibrate `ML_PER_REV` once the revised rotor and base are printed and tube is installed.
- Create housing for circuit board and mounts for the screen and keypad.

---

## 04/28/26 — Session 6: First Full Assembly Test

**Work completed**
- Confirmed all electrical components are functioning correctly together — motor runs, LCD displays correctly, keypad input works, cancel works.

**Issues**
- **No vacuum — tube not drawing water:** The tube is not being squeezed sufficiently to create a vacuum and draw water up. Tube clearance between the bearing and the outer wall needs to be reduced so the bearing compresses the tube fully.
- **Bearing lifting and squeezing off-center:** The bearing does not press straight down onto the tube — it shifts slightly to the side, meaning contact is not perpendicular throughout the rotation. This breaks the seal and prevents consistent peristaltic action.

**Notes**
- Electrically the system is complete and functioning; remaining issues are purely mechanical.
- Both problems are likely related — a bearing that can drift laterally will also fail to compress the tube consistently.

**Next**
- Increase bearing size to improve contact area and reduce the tendency to shift laterally.
- Add side walls to the rotor to constrain the bearing and tube laterally, keeping them aligned with each other throughout the full circumference.
- Adjust tube clearance so the bearing compresses the tube fully and creates a proper vacuum seal.
- Reprint rotor and retest.