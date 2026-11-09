# Model — CO2 Adsorption on 13X With Preloaded Water

Python version of the NASA model for CO2 uptake on Grade 544 (MS544) zeolite 13X when water is already adsorbed on the sorbent. This is the equilibrium model the test rig results will be compared against.

## Files

| File | What it does |
|---|---|
| `co2_h2o_13x_model.py` | The model. Checks the dry isotherm against NASA's measured data, prints CO2 uptake at 0.26 kPa for several water preloads, and plots `co2_h2o_13x_model.png`. |
| `water_effect_check.py` | Compares the paper's "half capacity lost at 1.25 mol/kg water" claim to its own equation and its measured data. Plots `water_effect_check.png`. |
| `data/tm2017_219847_13X_25C.csv` | Measured dry CO2 isotherm on 13X at 25 °C, from NASA/TM-2017-219847, Table 1. |
| `data/cmarik2018_fig9_normalized.csv` | Normalized CO2 uptake at 2 kPa vs. water preload at 25/50/75 °C, digitized from Cmarik & Knox Fig. 9 (about ±0.02). |
| `requirements.txt` | Python packages needed. |

## How to run

```
pip install -r requirements.txt
python co2_h2o_13x_model.py
python water_effect_check.py
```

## Model

1. **Dry CO2 capacity** (Cmarik & Knox Eq. 4): 3-site Langmuir isotherm.
   - n = Σ aⱼ·bⱼ·P / (1 + bⱼ·P), with aⱼ = a0ⱼ + c0ⱼ·T and bⱼ = b0ⱼ·exp(Eⱼ/T)
   - P in kPa, T in K. Parameters from NASA/TM-2017-219847, Table 2.
2. **Fraction of capacity lost to water** (Eq. 5): Toth form with water loading in place of pressure.
   - f = b·n_H2O / (1 + (b·n_H2O)^t)^(1/t), with b = 0.500 (mol/kg)⁻¹ and t = 1.251 (Table 1).
3. **Wet CO2 capacity** (Eq. 6): n_CO2 = n_dry × (1 − f).
4. **Linear model** (Eqs. 1–3), the paper's first attempt, is included for comparison. It hits zero capacity at 3.6 mol/kg water.

## Findings so far

**Dry isotherm**
- NASA's Table 2 parameters reproduce their 25 °C measurements to 0.016 mol/kg RMS (0.030 max) over 59 points.
- Dry capacity at 25 °C: 1.19 mol/kg at 0.26 kPa, 2.35 mol/kg at 2 kPa.

**Water effect: where is half of the CO2 capacity lost?**

| Source | Water loading (mol H2O/kg) |
|---|---|
| Paper's conclusion | 1.25 |
| Eq. 5 with Table 1 values | 1.55 |
| Measured, 25 °C | 2.0 |
| Measured, 50 °C | 1.4 |
| Measured, 75 °C | 1.2 |
| Eq. 5 refit to all three temperatures | 1.50 |
| Eq. 5 refit to 25 °C only | 1.91 |

- The Table 1 values behave like a fit across all three temperatures. The water model has no temperature term, so one curve covers all of them.
- The 1.25 in the paper's conclusion only matches the 75 °C data.
- At 25 °C the model predicts more capacity loss than was measured (by up to 0.07 of dry capacity). At room temperature it is conservative.
- The paper's TGA check (Fig. 10) also suggests a smaller water effect than the main FlexiDOSE data.

**Open questions**
- The low-preload points in Fig. 9 sit about 0.16 mol/kg left of the preloads listed in Fig. 8's legend (e.g. 0.65 vs. 0.81). Higher preloads match. Cause unknown.

## Limitations

- Equilibrium only: no time, airflow, or bed geometry. Predicting the sealed-chamber test (chamber CO2 vs. time) needs a chamber decay model built on this isotherm.
- 13X only. The rig's CO2 bed is 5A, which needs its own dry isotherm and water-effect parameters (Huang et al., ICES-2017-116).
- Assumes water removes the same fraction of capacity at every CO2 pressure.

## References

- Cmarik & Knox, *Co-Adsorption of Carbon Dioxide on Zeolite 13X in the Presence of Preloaded Water*, ICES-2018-3
- Cmarik, Son & Knox, *Standard Isotherm Fit Information for Dry CO2 on Sorbents for 4-Bed Molecular Sieve*, NASA/TM-2017-219847, https://ntrs.nasa.gov/citations/20180002395
