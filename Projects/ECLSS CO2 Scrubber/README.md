# ECLSS CO2 Scrubber

## Overview
A bench-scale CO2 scrubber based on the ISS Carbon Dioxide Removal Assembly (CDRA). The goal is to validate a home-built test method against published NASA sorbent data, then use it to size a working scrubber bed.

## Status
In progress. See the [engineering log](Notes/engineering_log.md) for session-by-session progress.

## Model
Python model of NASA's 13X CO2/water co-adsorption equations, checked against NASA's measured data. See [Model/README.md](Model/README.md) for what it does, how to run it, and findings so far.

## Plan
1. **Validate:** Measure CO2 uptake on zeolite 13X and compare to NASA's published data.
2. **Apply:** Measure zeolite 5A (CDRA's CO2 sorbent) with the same method and use the results to size a scrubber bed.

## Tools
| Category | Item |
|---|---|
| Modeling | Python |
| Electronics | Arduino, CO2/RH sensors |
| CAD | SolidWorks |
| Fabrication | 3D printing |

## References
- Cmarik & Knox, *Co-Adsorption of Carbon Dioxide on Zeolite 13X in the Presence of Preloaded Water*, ICES-2018-3
- Watson et al., *Sorbent Structural Testing on Carbon Dioxide Removal Sorbents for Advanced Exploration Systems*, ICES-2016-148
- Cmarik, Son & Knox, *Standard Isotherm Fit Information for Dry CO2 on Sorbents for 4-Bed Molecular Sieve*, NASA/TM-2017-219847
- Huang et al., *CO2 Capacity Sorbent Analysis Using Volumetric Measurement Approach*, ICES-2017-116