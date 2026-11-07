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
- 5A capacity was a placeholder (1 mol/kg) until real data was found.

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
- 1 person ≈ 1 kg CO2/day (NASA sources, verify before citing).

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