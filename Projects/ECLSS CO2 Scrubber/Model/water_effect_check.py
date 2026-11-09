"""
water_effect_check.py
---------------------
Checks the claim in Cmarik & Knox (ICES-2018-3), Section IV:

    "half of the CO2 capacity of dry 13X is lost when 1.25 mol/kg of
     water is adsorbed."

against (a) the paper's own Eq. 5 with Table 1 parameters, and
(b) the measured points in the paper's Fig. 9 (right panel), digitized
into data/cmarik2018_fig9_normalized.csv.

Fig. 9 shows CO2 uptake at 2 kPa divided by the dry value, at 25, 50
and 75 C. The water model has no temperature dependence, so a single
curve has to describe all three temperatures.

Run:  python water_effect_check.py
"""

from pathlib import Path

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit, brentq

HERE = Path(__file__).parent
B_TABLE1, T_TABLE1 = 0.500, 1.251
CLAIM = 1.25   # mol H2O/kg, from the paper's conclusion


def remaining(n_h2o, b, t):
    """1 - f_H2O from Eq. 5: fraction of dry CO2 capacity left."""
    x = b * np.asarray(n_h2o, dtype=float)
    return 1 - x / (1 + x**t) ** (1 / t)


def half_point(b, t):
    """Water loading where the model has lost half the CO2 capacity."""
    return brentq(lambda n: remaining(n, b, t) - 0.5, 0.01, 50)


def main():
    d = pd.read_csv(HERE / "data" / "cmarik2018_fig9_normalized.csv", comment="#")

    print(f"Paper's claim: half of capacity lost at {CLAIM} mol/kg\n")
    print(f"Eq. 5 with Table 1 (b={B_TABLE1}, t={T_TABLE1}):")
    print(f"  remaining at {CLAIM} = {remaining(CLAIM, B_TABLE1, T_TABLE1):.3f}")
    print(f"  half lost at       {half_point(B_TABLE1, T_TABLE1):.2f} mol/kg\n")

    print("Measured data (Fig. 9, linear interpolation between points):")
    for T, g in d.groupby("temp_C"):
        n, y = g.n_H2O_molkg.values, g.norm_CO2.values
        print(f"  {T} C: remaining at {CLAIM} = {np.interp(CLAIM, n, y):.3f}, "
              f"half lost at {np.interp(0.5, y[::-1], n[::-1]):.2f} mol/kg")

    print("\nRefitting Eq. 5 to the digitized data:")
    fits = {}
    for label, g in [("25 C only", d[d.temp_C == 25]), ("all temps", d)]:
        (b, t), _ = curve_fit(remaining, g.n_H2O_molkg, g.norm_CO2,
                              p0=[0.5, 1.25], bounds=([0.01, 0.2], [10, 10]))
        fits[label] = (b, t)
        print(f"  {label:9s}: b = {b:.3f}, t = {t:.3f}, "
              f"half lost at {half_point(b, t):.2f} mol/kg")

    # ---------------- Plot ----------------
    n = np.linspace(0, 9, 300)
    fig, ax = plt.subplots(figsize=(8, 5.5))
    for (T, g), c in zip(d.groupby("temp_C"), ["k", "tab:red", "tab:green"]):
        ax.plot(g.n_H2O_molkg, g.norm_CO2, "o", c=c, label=f"{T} C data (Fig. 9)")
    ax.plot(n, remaining(n, B_TABLE1, T_TABLE1), c="m", lw=2.5,
            label="Eq. 5, Table 1 values")
    b, t = fits["25 C only"]
    ax.plot(n, remaining(n, b, t), "k--", label="Eq. 5 refit, 25 C only")
    ax.axhline(0.5, c="gray", lw=0.8)
    ax.axvline(CLAIM, c="gray", lw=0.8, ls=":")
    ax.text(CLAIM + 0.08, 0.9, "paper's claim:\nhalf lost at 1.25", color="gray")
    ax.set(xlabel="Water preload (mol H2O/kg)",
           ylabel="CO2 uptake at 2 kPa / dry uptake",
           title="Where does 13X lose half its CO2 capacity?",
           xlim=(0, 9), ylim=(0, 1.05))
    ax.legend(fontsize=9)
    ax.grid(alpha=0.3)
    plt.tight_layout()
    plt.savefig(HERE / "water_effect_check.png", dpi=150)
    print("\nSaved plot -> water_effect_check.png")
    plt.show()


if __name__ == "__main__":
    main()
