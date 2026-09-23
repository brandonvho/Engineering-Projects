# Engineering Log – Mars Habitat CO2 Scrubber

---

## 09/22/26 — Session 1: Literature Review – ISS CDRA Background

**Work completed**
- Read two papers on the ISS Carbon Dioxide Removal Assembly (CDRA) to build foundational understanding before starting any modeling work:
  1. Cmarik & Knox, *"Co-Adsorption of Carbon Dioxide on Zeolite 13X in the Presence of Preloaded Water"* (NASA/MSFC, ICES-2018-3)
  2. Watson, Knox, West & Bush, *"Sorbent Structural Testing on Carbon Dioxide Removal Sorbents for Advanced Exploration Systems"* (NASA/MSFC, ICES-2016-148)

**Issues**
- None this session — both papers were freely accessible via NASA NTRS.

**Notes**
- **CDRA architecture (4-Bed Molecular Sieve, 4BMS):** Two pairs of sorbent beds. Each pair has a desiccant bed (silica gel bulk layer + 13X zeolite layer to strip residual water below ~1 ppm) upstream of a CO2 sorbent bed (zeolite 5A). Air passes through a drying bed, then a precooler, then the CO2-adsorbing bed, while the other pair simultaneously desorbs/regenerates via heating and vacuum. The half-cycles alternate so removal is continuous.
- **Why the desiccant layer matters so much:** 13X zeolite is not just a desiccant — it's also a strong CO2 adsorber. If the desiccant bed grabs CO2 that should have reached the dedicated CO2 bed, overall system CO2 removal rate drops significantly. One cited simulation found a fully non-CO2-adsorbing desiccant layer would improve time-averaged removal rate by ~50%, and a physical test cutting the 13X desiccant volume in half measured a ~20% real performance gain. This is a strong argument for sizing/choosing the desiccant layer carefully rather than just oversizing it "to be safe."
- **Water vs. CO2 co-adsorption on 13X:** Water is adsorbed far more strongly than CO2 on 13X — even small amounts of preloaded water sharply cut CO2 capacity. The paper's data: roughly half of dry 13X's CO2 capacity is lost after only ~1.25 mol/kg of water is adsorbed. They fit this with a Toth-isotherm-shaped empirical model where water loading (instead of pressure) suppresses a "dry" CO2 isotherm via a multiplicative factor. This is directly relevant to modeling my own bed — I'll need a similar two-component isotherm (not just a pure-CO2 Langmuir/Toth fit) if humidity is present in my habitat's air loop.
- **Practical isotherm modeling approach:** Their baseline dry CO2 isotherm was fit with a 3-site Langmuir-type sum (not a single-site Langmuir), then multiplied by a water-suppression factor. Good template for my own Python fitting work later — start with a dry isotherm fit, then decide if a humidity term is even needed for a Mars atmosphere (Mars cabin humidity vs. CO2 partial pressure needs to be pinned down before I know if this cross-effect matters much for my design).
- **Mechanical/structural side (sorbent durability):** Separate from adsorption chemistry, sorbent pellets have to survive repeated pressure/temperature cycling and humidity exposure without breaking down into dust ("fines"), which can clog retention screens and increase pressure drop / blower power draw. NASA runs standardized bulk crush, single-pellet crush, and attrition tests (ASTM D7084, D4179, D4058) at multiple humidity levels to screen candidate sorbents. Zeolite 13X (Grade 544) and the ISS's actual sorbent (ASRT) held up comparably well; some candidates (e.g., Grade 522 5A) degraded much more under humidity. This is a durability axis I hadn't been thinking about yet — my design will need to account for pellet attrition over repeated cycles, not just adsorption capacity.

**Next**
- Start narrowing down Mars-specific operating conditions (expected cabin CO2 partial pressure, relative humidity, temperature range) since these determine whether the humidity co-adsorption effect from paper 1 is significant for this design.
- Decide on a target sorbent (13X vs. 5A vs. something else) based on adsorption capacity + the structural/attrition data from paper 2.
- Look for a baseline dry CO2 isotherm data set (Langmuir/Toth parameters) to use as a starting point for the Python modeling phase.

---

## 09/22/26 — Session 2: Test Rig Staging Decisions – Standalone Scrubber, Desiccant/Sorbent Isolation, Fan Reconsideration
 
**Work completed**
- Decided to scope the first physical build as a standalone scrubber assembly rather than the full habitat enclosure — air in, through a desiccant stage, then a CO2 sorbent stage, with CO2/RH measured before and after each stage. This lets me validate each material's actual performance directly instead of inferring it from whole-system behavior.
- Chose to split desiccant and CO2-sorbent duty across two different materials rather than using 13X for both: silica gel handles water removal upstream, and 5A zeolite handles CO2 removal downstream. This directly follows the co-adsorption finding from Session 1 — since 13X adsorbs CO2 as well as water, using it as the desiccant stage would let some CO2 removal happen in the wrong bed, corrupting the CO2 mass balance I need for a clean measurement. Silica gel doesn't meaningfully adsorb CO2, so all CO2 capture stays attributable to the 5A bed.
- Already have a reusable, color-changing silica gel on hand (currently used for 3D print filament storage) that can serve as the desiccant — indicates saturation visually and regenerates by baking.
- Reconsidered using computer case fans for flow control. Case fans are axial fans designed for near-zero back-pressure applications (open airflow, heatsinks) and have very little static pressure capability. A packed sorbent bed presents real flow resistance, so a case fan likely can't push a known, steady flow rate through it — flow rate would be uncontrolled and could drop off unpredictably as the bed saturates, and there's a real risk of air channeling around the bed instead of through it (which would look like fast/false breakthrough in the data).

**Issues**
- Case fans ruled out as flow control hardware due to static-pressure mismatch with packed-bed resistance — need a different flow source before the build can proceed.

**Notes**
- Flow control alternatives to evaluate: small diaphragm/aquarium air pump, or a centrifugal/blower-style fan (e.g. 3D-printer part-cooling blower) — both handle static pressure much better than axial fans. Pair with a needle valve for coarse control and a rotameter or small MFC for actual flow rate measurement, since flow rate feeds directly into the breakthrough curve / mass balance modeling later.
- Sensor placement plan: three CO2/RH readers — inlet, between the desiccant and 5A beds, and outlet. The mid-point reading is the important addition: it directly confirms whether the desiccant bed is contributing to the CO2 signal at all, which validates (or disproves) the isolation assumption behind the whole two-material split.
- CO2 sensor candidates under consideration: MH-Z19B/C (cheap NDIR, standard range up to ~5000 ppm) vs. SCD30/SCD40 (NDIR CO2 + RH/temp in one module, range up to ~40,000 ppm). Which one is viable depends on what inlet CO2 concentration I actually test at — near-ambient (~400-500 ppm) vs. a simulated/spiked habitat concentration in the thousands of ppm (closer to the ~0.26 kPa target partial pressure referenced in the Cmarik & Knox paper).

**Next**
- Source flow control hardware (pump or blower) and a flow measurement device.
- Size the silica gel and 5A beds — mass and dimensions — once target flow rate and target inlet CO2 concentration are pinned down, using 5A working capacity data from isotherm literature.
- Pick CO2/RH sensor(s) once target concentration range is decided.
- Design ducting/tubing routing between stages (inlet → desiccant → mid sensor → 5A → outlet sensor).
---