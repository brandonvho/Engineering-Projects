"""
co2_h2o_13x_model.py
--------------------
Python version of the CO2 / H2O co-adsorption model from:

    Cmarik, G. E. & Knox, J. C., "Co-Adsorption of Carbon Dioxide on
    Zeolite 13X in the Presence of Preloaded Water," ICES-2018-3.

Sorbent: Grace-Davison MS544 (Grade 544) 13X.

What the model says (paper Eqs. 4-6):
    1. Dry CO2 capacity n*_CO2(P, T) comes from a 3-site Langmuir isotherm (Eq. 4).
    2. Preloaded water knocks out a fraction f_H2O of that capacity.
       f_H2O follows a Toth-shaped curve, but with WATER LOADING in place
       of pressure (Eq. 5).
    3. Actual CO2 uptake = dry capacity * (1 - f_H2O)            (Eq. 6)

Dry isotherm parameters are NASA's own fit, from the paper's Ref. 9:
    Cmarik, Son & Knox, "Standard Isotherm Fit Information for Dry CO2 on
    Sorbents for 4-Bed Molecular Sieve," NASA/TM-2017-219847, Table 2.
    https://ntrs.nasa.gov/citations/20180002395
The measured 25 C data from that report (Table 1) is in data/ and is
plotted as a check on the parameters.

Run:  python co2_h2o_13x_model.py
Needs: numpy, pandas, matplotlib   (pip install -r requirements.txt)
"""

from pathlib import Path

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

HERE = Path(__file__).parent

# ---------------------------------------------------------------------
# 1. INPUTS
# ---------------------------------------------------------------------

T_K = 298.15     # 25 C, the temperature of the paper's Fig. 8

# Dry 3-site Langmuir parameters, NASA/TM-2017-219847 Table 2 (Grade 544 13X)
#   a_j = a0_j + c0_j * T        site capacity (mol/kg)
#   b_j = b0_j * exp(E_j / T)    site affinity (1/kPa), E_j in K
B0 = np.array([5.057e-8, 8.201e-8, 5.032e-9])
E  = np.array([5584.0,   4408.0,   4408.0])
A0 = np.array([1.640,    2.537,    1.995])
C0 = np.array([8.350e-9, 1.417e-8, -2.668e-9])

# Water-effect parameters, Cmarik & Knox Table 1 (Toth-form model)
B_H2O = 0.500   # (mol H2O/kg)^-1
T_H2O = 1.251   # dimensionless Toth heterogeneity parameter

# Linear model parameter, Cmarik & Knox Eqs. 1-3 (their first, simpler attempt)
N_STAR_H2O = 3.6   # mol H2O/kg -> linear model says CO2 capacity hits 0 here

# Water preloads to plot (mol H2O/kg) -- same values as the paper's Fig. 8
PRELOADS = [0.0, 0.81, 1.17, 1.74, 2.35, 4.30, 6.11, 9.93]

P_CABIN = 0.26   # kPa, target CO2 partial pressure for crewed missions (paper)
P_COMPARE = 2.0  # kPa, the pressure the paper uses for Fig. 9


# ---------------------------------------------------------------------
# 2. MODEL EQUATIONS
# ---------------------------------------------------------------------

def dry_co2(P, T=T_K):
    """Eq. 4: 3-site Langmuir. Each site j has a capacity a_j (mol/kg)
    and an affinity b_j (1/kPa). Total = sum of the three sites.
    P in kPa, T in K."""
    P = np.asarray(P, dtype=float)[..., None]   # broadcast over the 3 sites
    a = A0 + C0 * T
    b = B0 * np.exp(E / T)
    return np.sum(a * b * P / (1 + b * P), axis=-1)


def f_water_toth(n_h2o, b=B_H2O, t=T_H2O):
    """Eq. 5: fraction of CO2 capacity lost to preloaded water.
    Goes from 0 (dry) toward 1 (fully blocked) but never reaches it."""
    x = b * np.asarray(n_h2o, dtype=float)
    return x / (1 + x**t) ** (1 / t)


def f_water_linear(n_h2o, n_star=N_STAR_H2O):
    """Eq. 1: simpler linear capacity loss, clipped to [0, 1]."""
    return np.clip(np.asarray(n_h2o, dtype=float) / n_star, 0, 1)


def co2_uptake(P, n_h2o, T=T_K, f_func=f_water_toth):
    """Eq. 6 (or Eq. 2): CO2 uptake with water preloaded."""
    return dry_co2(P, T) * (1 - f_func(n_h2o))


# ---------------------------------------------------------------------
# 3. RUN
# ---------------------------------------------------------------------

def main():
    # Check the NASA parameters against NASA's own measured 25 C data
    dry = pd.read_csv(HERE / "data" / "tm2017_219847_13X_25C.csv", comment="#")
    resid = dry.n_molkg - dry_co2(dry.P_kPa)
    print("Dry isotherm check vs NASA/TM-2017-219847 measured data (25 C):")
    print(f"  RMS error = {np.sqrt(np.mean(resid**2)):.3f} mol/kg, "
          f"max error = {np.abs(resid).max():.3f} mol/kg "
          f"over {len(dry)} points\n")

    print(f"Dry capacity at {P_CABIN} kPa = {dry_co2(P_CABIN):.3f} mol/kg")
    print(f"Dry capacity at {P_COMPARE} kPa  = {dry_co2(P_COMPARE):.3f} mol/kg\n")

    # Table of predictions at cabin CO2 level
    print(f"Predicted CO2 uptake at {P_CABIN} kPa (mol CO2/kg):")
    print(f"  {'H2O preload':>12s} {'Toth model':>11s} {'Linear model':>13s} {'f_H2O':>7s}")
    for nw in PRELOADS:
        toth = co2_uptake(P_CABIN, nw, f_func=f_water_toth)
        lin = co2_uptake(P_CABIN, nw, f_func=f_water_linear)
        print(f"  {nw:12.2f} {toth:11.3f} {lin:13.3f} {f_water_toth(nw):7.3f}")

    print(f"\nf_H2O at 1.25 mol/kg = {f_water_toth(1.25):.3f}  "
          "(paper's conclusion says 0.5 -- see water_effect_check.py)")

    # ---------------- Plots ----------------
    P = np.linspace(0.001, 6.0, 400)
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(13, 5))

    # Left: isotherm family, like paper Fig. 8
    d6 = dry[dry.P_kPa <= 6]
    ax1.plot(d6.P_kPa, d6.n_molkg, "ko", ms=4, label="Dry, measured (NASA TM)")
    for nw in PRELOADS:
        ax1.plot(P, co2_uptake(P, nw), label=f"{nw:.2f} mol H2O/kg")
    ax1.axvline(P_CABIN, ls="--", c="gray", lw=1)
    ax1.text(P_CABIN + 0.05, 3.1, "cabin 0.26 kPa", color="gray")
    ax1.set(xlabel="CO2 partial pressure (kPa)", ylabel="CO2 uptake (mol/kg)",
            title="MS544 13X, 25 C — Toth water model (Eq. 6)",
            xlim=(0, 6), ylim=(0, 3.3))
    ax1.legend(fontsize=8, loc="lower right")
    ax1.grid(alpha=0.3)

    # Right: capacity vs water preload at fixed P, like paper Fig. 9
    nw = np.linspace(0, 10, 300)
    for Pc, style in [(P_COMPARE, "-"), (P_CABIN, "--")]:
        ax2.plot(nw, co2_uptake(Pc, nw, f_func=f_water_toth), style,
                 label=f"Toth, {Pc} kPa")
        ax2.plot(nw, co2_uptake(Pc, nw, f_func=f_water_linear), style,
                 alpha=0.5, label=f"Linear, {Pc} kPa")
    ax2.set(xlabel="Water preload (mol H2O/kg)", ylabel="CO2 uptake (mol/kg)",
            title="CO2 capacity lost to water", xlim=(0, 10), ylim=(0, None))
    ax2.legend(fontsize=8)
    ax2.grid(alpha=0.3)

    plt.tight_layout()
    plt.savefig(HERE / "co2_h2o_13x_model.png", dpi=150)
    print("\nSaved plot -> co2_h2o_13x_model.png")
    plt.show()


if __name__ == "__main__":
    main()
