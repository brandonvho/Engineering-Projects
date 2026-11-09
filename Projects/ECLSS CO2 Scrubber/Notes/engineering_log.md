# Engineering Log – ECLSS CO2 Scrubber

---

## 09/22/26 — Session 1: Literature Review

**Work completed**
- Read two NASA papers on the ISS CO2 Removal Assembly (CDRA): Cmarik & Knox (CO2 on 13X with water) and Watson et al. (sorbent durability).

**Issues**
- None.

**Notes**
- CDRA dries the air first (silica gel + 13X), then removes CO2 with 5A zeolite.
- Water sticks to zeolite more strongly than CO2 and cuts its CO2 capacity, so the air has to be dried first.
- Zeolite beads can break into dust over many cycles, which clogs screens and raises pressure drop.

**Next**
- Pick test conditions and a sorbent.
- Find published isotherm data to model against.

---

## 09/22/26 — Session 2: Test Rig Layout

**Work completed**
- Decided to build a standalone test rig first: silica gel bed, then 5A bed, with a CO2 sensor at the inlet, between the beds, and at the outlet.
- Using silica gel for drying instead of 13X, since silica gel doesn't capture CO2. All CO2 removal stays in the 5A bed.

**Issues**
- Computer case fans can't push air through a packed bed. Need a different flow source.

**Notes**
- The middle sensor confirms the silica gel isn't removing CO2.

**Next**
- Choose a flow source, size the beds, pick sensors.

---

## 09/23/26 — Session 3: First Bed Sizing

**Work completed**
- First sizing: 3 L/min, 8,000 ppm inlet, 30-minute test → about 30 g of 5A in a 25 mm tube, 8 cm deep.
- Estimated pressure drop with the Ergun equation: about 50 Pa.

**Issues**
- 5A capacity was a placeholder (1 mol/kg) until real data was found (resolved in Session 5).

**Notes**
- Regenerate by baking the 5A between runs.

**Next**
- Find real 5A capacity data.
- Pick CO2 sensors that read up to 8,000+ ppm.

---

## 10/02/26 — Session 4: Flow Source and Scale

**Work completed**
- Chose a single blower at the inlet (later decided on a centrifugal blower).
- Decided to start at 1/10 of one person's CO2 output so mistakes are cheap.

**Notes**
- 1 person ≈ 1 kg CO2/day (NASA sources, verify before citing). 1/10 scale ≈ 100 g/day ≈ 0.095 mol/hr.

**Next**
- Decide the test setup.

---

## 10/30/26 — Session 5: Test Method Redesign

**Work completed**
- Ruled out using my room as a test chamber: too large and leaky to measure.
- Changed to a sealed chamber filled with my own breath, with the scrubber circulating the air in a closed loop.
- Decided to test 13X first and compare it to NASA data to prove the method works, then test 5A.
- Found the sources for this: NASA/TM-2017-219847 (13X model) and Huang et al. ICES-2017-116 (measured 13X and 5A data).

**Issues**
- The NASA model report only covers 13X. 5A parameters will have to be fit from the Huang data.

**Notes**
- Real 5A capacity at ~0.8 kPa is about 1.3 mol/kg, so the 1 mol/kg placeholder was conservative.
- Oven baking (~250°C, no vacuum) will likely give lower capacity than NASA's 350°C vacuum activation.

**Next**
- Choose chamber volume and sample mass.
- Start the Python isotherm model.

---

## 11/04/26 — Session 6: Scope Change

**Work completed**
- Dropped the Mars adaptation. The project is now Earth-based CDRA validation.

**Notes**
- Mars conditions can't be recreated at home, so the project focuses on what I can build and measure.

**Next**
- Write one pass/fail requirement for matching the 13X data.

---

## 11/08/26 — Session 7: First Python Model (13X)

**Work completed**
- Wrote my first Python model: `Model/co2_h2o_13x_model.py`. It recreates the Cmarik & Knox 13X CO2/water co-adsorption model.
  - Dry CO2 capacity: 3-site Langmuir isotherm (Eq. 4) with NASA's parameters from NASA/TM-2017-219847.
  - Water penalty: Toth-form fraction of capacity lost (Eq. 5, b = 0.500, t = 1.251 from Table 1).
  - Wet CO2 capacity = dry capacity × (1 − f_H2O) (Eq. 6).
  - Also included the paper's simpler linear water model (Eqs. 1–3) for comparison.
- Plotted CO2 uptake vs. CO2 pressure at the paper's water preloads, and CO2 uptake vs. water preload at 0.26 kPa and 2 kPa (`Model/co2_h2o_13x_model.png`).
- Added `Model/requirements.txt` (numpy, scipy, pandas, matplotlib) and source data in `Model/data/`.

**Issues**
- The paper doesn't list the dry isotherm parameters. First version of the script used dry data eyeballed from Fig. 5/8.
  - Resolved: found NASA/TM-2017-219847 (the paper's Ref. 9). Its Table 2 has NASA's 3-site Langmuir parameters and Table 1 has the measured data. The parameters reproduce the 25 C data to 0.016 mol/kg RMS (pressure in kPa, T in K). The model now uses them directly.
  - Dry capacity at 0.26 kPa is 1.19 mol/kg. The eyeballed version said 1.28, about 7% high.
- Eq. 5 with the Table 1 values gives f_H2O = 0.44 at 1.25 mol/kg, but the paper's conclusion says half is lost at 1.25.
  - Checked against Fig. 9 data (digitized, `Model/water_effect_check.py`). The equation hits half at 1.55 mol/kg. The data hits half at 2.0 (25 C), 1.4 (50 C) and 1.2 (75 C).
  - A refit of Eq. 5 to all three temperatures gives about 1.5, so the Table 1 values behave like an all-temperature fit. The 1.25 in the conclusion only matches the 75 C data.
  - At 25 C the model predicts more water damage than was measured (up to 0.07 of dry capacity), so it's conservative at room temperature.
  - The paper's TGA check (Fig. 10) also suggests water is less harmful than the FlexiDOSE data shows.

**Notes**
- Low-preload points in Fig. 9 sit about 0.16 mol/kg left of the preloads listed in Fig. 8's legend (e.g. 0.65 vs 0.81). Higher preloads match. Not sure why.
- A small amount of water does the most damage: ~0.8 mol/kg (~1.5 wt%) cuts CO2 capacity by about a third. The silica gel matters most right as water starts breaking through.
- The linear model drops to zero capacity at 3.6 mol/kg water. The Toth model never reaches zero.
- The model assumes water removes the same fraction of capacity at every CO2 pressure.
- This is an equilibrium model only — no time or airflow. The sealed-chamber test (Session 5) measures chamber CO2 dropping over time, so the next model is a chamber decay model. Uptake comes from a mass balance: change in chamber CO2 × chamber volume ÷ sorbent mass.

**Next**
- Write the pass/fail requirement: measured 13X uptake within X% of NASA/TM-2017-219847 at the same CO2 pressure (carried over from Session 6).
- Choose chamber volume and 13X sample mass (carried over from Session 5).
- Build a chamber decay model: chamber CO2 vs. time for a given volume, flow rate, and sample mass, using the NASA dry isotherm.
- Measure the blower's actual flow through the packed bed.
- Size the silica gel bed from flow, room humidity, and run time.

---
