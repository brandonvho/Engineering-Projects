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
- **`*` emergency stop not working after `AccelStepper` switch:** `myStepper.stop()` decelerates gradually rather than halting instantly, so `distanceToGo()` remained non-zero and the done/cancelled branch in `runOneStep()` never triggered. Fixed by replacing `myStepper.stop()` with `myStepper.setCurrentPosition(myStepper.currentPosition())`, which immediately zeros `distanceToGo()`.
- **Motor shaking and fighting itself:** The coils were firing in the wrong sequence, causing them to oppose each other. Root cause was a mismatch between `AccelStepper`'s expected pin order and the physical ULN2003 wiring. Fixed by physically swapping the D9 and D10 wires on the ULN2003 board rather than changing the code.
- **Unwanted acceleration and deceleration:** `AccelStepper.run()` acceleration ramp felt unnatural for a pump application. Fixed by switching to `runSpeedToPosition()` with a fixed `setSpeed(400)` called after `move()`, which runs at constant speed with no ramp and ignores `setAcceleration()` entirely.
- **RW pin — LCD blank on first breadboard build:** The RW (read/write) pin on the LCD1602 was left unconnected. It must be tied to GND for write-only operation — leaving it floating prevents any display output regardless of all other wiring being correct.

**Notes**
- `AccelStepper` with `runSpeedToPosition()` and `setSpeed()` is now the correct pattern for this use case — constant speed, no ramp, respects `distanceToGo()`.
- Pin order on the ULN2003 must be verified against `AccelStepper`'s expected sequence whenever rewiring — coil fight is silent and hard to diagnose.
- Speed was set to `setSpeed(350)` — determined to be the fastest speed at which the motor retains enough torque to run without stalling.

**Next**
- Calibrate `ML_PER_REV` once the revised rotor and base are printed and tube is installed.
- Create housing for circuit board and mounts for the screen and keypad.

---

## 04/28/26 — Session 6: First Liquid Pump Test

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

## 05/02/26 — Session 7: Bearing & Rotor Arm Redesign — Root Cause Analysis and Design Point Selection
 
**Work completed**
- Identified arm radius as the root cause of pump failure — not housing clearance.
- Evaluated all five bearing candidates against torque, geometry, and structural constraints.
- Selected 5×11×5 mm bearing at r = 9 mm as the recommended design point.
- Updated sketch with new operating speed.

**Root cause: torque budget** 
Session 6 confirmed the motor, LCD, keypad, and cancel all work correctly, but the pump failed to draw water. Two symptoms were observed: insufficient tube occlusion and off-centre bearing contact. The mechanical root cause is the rotor arm radius, not the housing clearance — tightening clearance only increases the required compression force, which the motor cannot supply at r = 25 mm.
 
Motor torque-speed model (linear approximation):
 
```
T_avail = 34.3 × (1 − RPM / 29.3)   [mN·m]
Stall torque:       34.3 mN·m  (at 0 RPM)
Zero-torque speed:  29.3 RPM   (at 1000 Hz step frequency, half-step mode)
```
 
At current operating point (`setSpeed(350)` → 10.25 RPM):
 
```
T_avail = 34.3 × (1 − 10.25 / 29.3) = 22.3 mN·m
T_req   = Fc × r = 2.0 N × 25 mm   = 50.0 mN·m
Deficit = 50.0 − 22.3              = 27.7 mN·m
Max safe Fc at r = 25 mm           = 22.3 / 25 = 0.89 N
Full occlusion requires Fc ≈ 2.0 N → motor cannot comply at any speed
```
 
Reducing clearance to force occlusion would stall the motor before the tube fully closes. The arm radius must be reduced, which requires a smaller bearing so rollers do not overlap at 120° spacing.
 
**Bearing candidate evaluation**
Governing equations:
 
```
Fc       = 2.0 × (L / 5)   [N]    — compression force scaled by contact width L
                                      relative to 5 mm reference (E_silicone ≈ 2 MPa,
                                      δ = 1.5 mm occlusion, thin-shell ring model)
T_req    = Fc × r_arm       [mN·m] — torque required (1 active roller)
r_min    = OD_bearing / √3  [mm]   — minimum arm radius, 3 rollers at 120°, no overlap
gap      = r × √3 − OD      [mm]   — inter-roller edge clearance
RPM_max  = 29.3 × (1 − (T_req + 8) / 34.3)   — max RPM for 8 mN·m margin
sps      = RPM / 60 × 2048         — AccelStepper steps/sec
Q        = π × 1.5² × 2π × r × (RPM / 60) / 1000   [mL/min]
```
 
| Bearing (ID×OD×W) | r_arm | Fc (N) | T_req (mN·m) | T_avail (mN·m) | Margin (mN·m) | sps | RPM | Q (mL/min) | Peg ID | Gap (mm) | Verdict |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 3×8×4 mm | 8 mm | 1.6 | 12.8 | 22.6 | 9.8 | 341 | 10.0 | 3.56 | 3 mm | 5.86 | Fragile peg |
| 3×10×4 mm | 10 mm | 1.6 | 16.0 | 24.9 | 8.9 | 273 | 8.0 | 3.56 | 3 mm | 7.32 | Fragile peg |
| **5×11×5 mm** | **9 mm** | **2.0** | **18.0** | **26.1** | **8.1** | **239** | **7.0** | **2.80** | **5 mm** | **4.59** | **Recommended** |
| 7×14×5 mm | 10 mm | 2.0 | 20.0 | 28.4 | 8.4 | 171 | 5.0 | 2.22 | 7 mm | 3.32 | 2nd choice |
| 7×22×7 mm | ≥15 mm | 2.8 | ≥42.0 | 34.3 max | Deficit | — | — | — | 7 mm | — | Eliminated |
| 4×13×5 mm (current) | 25 mm | 2.0 | 50.0 | 22.3 | −27.7 | 350 | 10.3 | 11.4 | 4 mm | 4.32 | Root cause |
 
**Elimination reasoning**
- **7×22×7 mm — exceeds motor stall torque:** The 22 mm OD forces r_min = 12.7 mm. At that radius T_req = 35.6 mN·m, which exceeds the motor stall torque of 34.3 mN·m. No operating point exists — eliminated.
```
r_min = 22 / √3          = 12.7 mm
Fc    = 2.0 × (7/5)      = 2.8 N
T_req = 2.8 × 12.7       = 35.6 mN·m  >  stall torque of 34.3 mN·m
```

- **3×8×4 mm and 3×10×4 mm — deprioritised on spring arm fatigue grounds:** Both show better torque margin and flow than the recommended option but require a 3 mm mounting peg. Static peg safety factors are adequate (see Stress Analysis below), but the zig-zag spring arm fatigue safety factor at the thinnest cross-section drops to 2.7 — insufficient margin for continuous cyclic loading once print voids and layer-line weakness are considered. Deprioritised, not eliminated; usable if a solid arm profile is substituted.

**Stress analysis: peg and rotor arm**
Two structures are analysed separately: the **peg** (cylindrical post the bearing mounts on) and the **rotor arm** (cantilever from motor hub to bearing).
 
*Peg bending stress* — model: cantilever beam, circular cross-section, point load at bearing centre.
 
```
M_root    = Fc × (L_bearing / 2)
Z         = π × d³ / 32            (section modulus, solid cylinder)
σ_bending = M_root / Z
τ_shear   = Fc / (π × d² / 4)      (direct shear at root)
σ_vm      ≈ √(σ_bending² + 3τ²)    (von Mises)
```
 
| Bearing | d_peg | Fc (N) | M_root (N·mm) | Z (mm³) | σ_b (MPa) | τ (MPa) | σ_vm (MPa) | SF bulk | SF layer-line |
|---|---|---|---|---|---|---|---|---|---|
| 5×11×5 mm ★ | 5 mm | 2.0 | 5.0 | 12.27 | 0.41 | 0.10 | 0.42 | 119 | 60 |
| 3×8×4 mm | 3 mm | 1.6 | 3.2 | 2.65 | 1.21 | 0.23 | 1.23 | 41 | 20 |
 
Material properties (SF = safety factor = yield strength / stress):
- PLA bulk tensile yield: 50 MPa
- Inter-layer adhesion: ~25 MPa (~50% of bulk) — peg printed as vertical cylinder; occlusion force acts perpendicular to layer bonding (weakest direction for FDM)
- Neither peg approaches yield failure under static load.
*Rotor arm bending stress* — the arm is a cantilever of length r_arm loaded at the tip by Fc. Critical section is at the arm root.
 
```
M_arm = Fc × r_arm
I     = w × h³ / 12    (rectangular section, bending about weak axis)
c     = h / 2
σ_arm = M_arm × c / I
```
 
Case A — solid rectangular arm:
 
| Bearing | r_arm | M (N·mm) | Section w×h | I (mm⁴) | σ (MPa) | SF bulk | SF layer |
|---|---|---|---|---|---|---|---|
| 5×11×5 mm ★ | 9 mm | 18.0 | 8×5 mm | 83.3 | 0.54 | 93 | 46 |
| 3×8×4 mm | 8 mm | 12.8 | 6×3 mm | 13.5 | 1.42 | 35 | 18 |
 
Case B — zig-zag spring arm at minimum cross-section (Session 4 design intent):
 
| Bearing | r_arm | M (N·mm) | Strip w×h | I (mm⁴) | σ (MPa) | SF bulk | SF layer | SF fatigue¹ |
|---|---|---|---|---|---|---|---|---|
| 5×11×5 mm ★ | 9 mm | 18.0 | 5×2.5 mm | 6.51 | 3.46 | 14.5 | 7.2 | 5.1 |
| 3×8×4 mm | 8 mm | 12.8 | 3×2 mm | 2.0 | 6.40 | 7.8 | 3.9 | 2.7 |
 
¹ PLA fatigue endurance limit estimated at 35% of UTS ≈ 17.5 MPa (no official S-N data for FDM PLA — treat as approximate). SF_fatigue = 17.5 / σ_max.
 
The spring arm fatigue case — not peg yield — is the actual differentiating failure mode. The 3 mm peg is statically safe (SF = 41); it is deprioritised because the arm's minimum cross-section under thousands of cyclic compression events gives SF = 2.7, which is insufficient once print voids and layer-line weakness are considered.
 
**Recommended design point — full worked calculations (5×11×5 mm at r = 9 mm)**
```
r_min          = 11 / √3                   = 6.35 mm  →  use 9 mm
Inter-gap      = 9 × √3 − 11              = 4.59 mm  (rollers clear, tube fits)
Fc             = 2.0 × (5/5)              = 2.0 N
T_req          = 2.0 × 9                  = 18.0 mN·m
sps            = 7.0 / 60 × 2048         = 239 steps/sec
T_avail        = 34.3 × (1 − 7.0/29.3)   = 26.1 mN·m
Margin         = 26.1 − 18.0             = 8.1 mN·m  (31% headroom)
Q              = π×1.5² × 2π×9 × (7.0/60) / 1000  = 2.80 mL/min
R_housing      ≈ r_arm + OD_bearing/2 + tube_wall_clearance − tube_wall
               = 9 + 5.5 + 5 − 1.5       = 18 mm
Housing OD     ≈ 36 mm  (vs current ~70 mm)
 
Peg σ_bending  = (2.0 × 2.5) / (π×5³/32) = 0.41 MPa   SF = 119
Arm σ (spring) = (18.0 × 1.25) / 6.51    = 3.46 MPa   SF_fatigue = 5.1
 
Design note: bearing inner bore (5 mm) = tube OD (5 mm) — shared reference
dimension simplifies housing geometry and peg sizing.
```
 
Reducing arm radius from 25 mm to 9 mm shrinks the housing inner diameter from ~70 mm to ~36 mm — significant reduction in footprint, print time, and material.
 
**Issues**
| Area | Issue | Resolution |
|---|---|---|
| Root cause | Arm radius too large — motor torque deficit of 27.7 mN·m at r = 25 mm | Reduce arm radius; select smaller bearing |
| 7×22×7 mm bearing | T_req exceeds motor stall torque at minimum permissible radius | Eliminated |
| 3 mm peg bearings | Spring arm fatigue SF = 2.7 — insufficient for cyclic load | Deprioritised; viable with solid arm profile |
| Housing size | Current ~70 mm ID housing oversized for new rotor geometry | Housing inner diameter drops to ~36 mm at new design point |
 
**Sketch changes made**
```cpp
// Before
myStepper.setSpeed(350);   // 10.25 RPM, r = 25 mm, T_req > T_avail
 
// After
myStepper.setSpeed(239);   // 7.0 RPM, r = 9 mm, 31% torque margin
// STEPS_PER_REV unchanged (2048, half-step mode)
// ML_PER_REV must be recalibrated after reprint
```
 
**Notes**
- 5×11×5 mm bearing at r = 9 mm is the primary choice — smallest bearing that clears the peg stress constraint, fits geometrically at 120° spacing, and delivers 31% torque margin.
- 7×14×5 mm at r = 10 mm, 171 sps is the fallback — higher peg strength (7 mm ID), 8.4 mN·m margin, 2.22 mL/min flow. Preferred if the 5 mm peg proves insufficient under sustained load.
- Bearing inner bore (5 mm) matches tube OD (5 mm) — convenient shared reference dimension for housing geometry and peg sizing.
**Next**
- Redesign rotor in Fusion: r = 9 mm arm, 5 mm peg diameter, zig-zag spring profile with minimum strip section ≥ 5×2.5 mm.
- Reprint rotor and motor base.
- Adjust housing clearance so bearing OD compresses tube 1.5 mm against wall (full occlusion).
- Upload sketch with `setSpeed(239)`.
- Empirically measure Fc: press one bearing into tube on a kitchen scale, record force at lumen closure.
- Calibrate `ML_PER_REV` once assembly is complete.