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