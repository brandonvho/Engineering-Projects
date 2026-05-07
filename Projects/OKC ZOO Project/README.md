# OKC Zoo — Darkling Beetle Habitat Enrichment Hide

**Group 9 | Brandon Ho, Bryce Dodson, Jadyn Goodson-Pelayo, Pablo Vazquez**  
*8 April 2026*

---

## Overview

This project involved designing a habitat enrichment hide for Darkling Beetles at the Oklahoma City Zoo. The zoo's existing environment — cardboard tubes and egg cartons — could not be sanitized, lacked structural complexity, and depended on inconsistent donations. The goal was to create a reusable, natural-looking hide that better supports the beetles' instinctive behaviors such as burrowing, dog-piling, tunneling, and seeking dark spaces.

---

## Objectives

- Develop skills in CAD modeling, rapid prototyping, and iterative design
- Apply a weighted decision matrix to evaluate and select among multiple design concepts
- Design and deliver a functional enrichment hide that meets all client requirements and passes a structured testing plan

---

## Requirements & Constraints

| Category | Details |
|---|---|
| **Size** | Must fit within small bins (12″ × 6″) and large bins (18″ × 12″); max 10″ long, 4″ wide, 4″ tall |
| **Cleanability** | Must disassemble and clean daily (every 24 hours) |
| **Ventilation** | Must prevent deadly dog-piling and maintain 68–72°F habitat temperature |
| **Entrances/Exits** | Multiple required — beetles cannot reverse well |
| **Surface Texture** | Must be rough/textured so beetles can grip and climb |
| **Dark Chambers** | Must provide dark internal spaces for breeding, egg laying, and stress reduction |
| **Appearance** | Must look natural to mimic the beetles' wild environment |
| **Materials** | No wood, paint, sealant, stain, or chemicals of any kind; non-toxic; no staples, plastic tape, or treated materials |
| **Cost** | $5–$10 in materials |

---

## Ideation & Design Selection

Each team member brainstormed three designs, ranging from honeycomb modular enclosures and ant-farm-style rock shapes to multi-level box structures and log hides. The team evaluated concepts using a **weighted decision matrix** with the following criteria:

| Criteria | Weight |
|---|---|
| Size / Space | 40% |
| Cleanability | 30% |
| Natural Look | 20% |
| Elevation | 10% |

The top four candidates were scored against these criteria:

| Design | Total Score | Rank |
|---|---|---|
| **Fake Log** | 4.5 | 🥇 1st |
| Hotel | 3.3 | 2nd |
| Ant Farm | 2.7 | 3rd |
| Honeycomb | 2.2 | 4th |

The **Fake Log** won decisively due to its natural appearance, ease of cleaning, and strong performance across all weighted criteria.

---

## Design Summary

The final design is a two-piece 3D-printed log hide modeled in **Fusion 360**.

- **Bottom tray** — A half-cylinder (8″ × 4″) that holds leaf litter and substrate for burrowing
- **Top shell** — A matching half-cylinder that slides onto the tray via a rail-and-groove connection, creating a fully enclosed log shape when assembled
- **Internal chambers** — Curved walls inside the top shell form multiple dark chambers at different elevations
- **Ventilation** — Slits along the right side of the top piece (and bottom piece in the final version) maintain airflow
- **Entrances/exits** — Several holes along the sides allow beetles to pass through without reversing
- **Burial depth** — ~1.5″ buried in substrate, leaving ~2.5″ of usable above-ground height
- **Texture** — Bambu Lab Studio's "fuzzy skin" print setting generates a rough exterior for beetle grip

---

## Tools & Technologies

| Tool | Use |
|---|---|
| **Fusion 360** | Part modeling and engineering drawings |
| **Bambu Lab 3D Printer** | Printing in brown PLA filament |
| **Bambu Lab Studio** | Fuzzy skin texture setting for final print |
| **Cardboard & Tape** | Rapid prototyping to verify scale and fitment |

---

## Prototype & Testing

The team built a cardboard rapid prototype first to verify scale, then 3D-printed a proof-of-concept prototype in brown PLA. Six requirements were tested on both the initial prototype and the final design.

### Initial Prototype Results

| Requirement | Test Method | Pass/Fail | Notes |
|---|---|---|---|
| Size (max 10″ × 4″) | Ruler measurement | ✅ Pass | 8″ long, 4″ diameter |
| Cleanability | Assemble/disassemble fitment test | ✅ Pass | Pieces slide together and apart easily |
| Surface Texture | Place real bugs on design and observe | ❌ Fail | Bugs slid off smooth PLA surface |
| Elevation | Place real bugs on design and observe climbing | ❌ Fail | Bugs could not grip sides to change elevation |
| Ventilation | Dry-ice fog test with vent holes | ✅ Pass | Smoke visible exiting top vent slits |
| Strength | Drop test | ✅ Pass | No breakage |

**4 of 6 tests passed.** Both failures traced back to the smooth PLA surface.

---

## Client Feedback & Revisions

After the prototype review, the zoo's caretakers requested four changes before the final print:

1. **Texture on both pieces** — Only the top had been printed with fuzzy skin; the bottom needed it too
2. **Reduced wall thickness** — Lower material cost and print time without sacrificing strength
3. **Improved connection** — A flat alignment guide added to the first 0.5″ of the sliding joint for faster, intuitive reassembly when managing many bins
4. **Ventilation on the bottom piece** — Slits added so beetles burrowed beneath the tray still receive airflow

All four changes were incorporated in Fusion 360 before the final print.

---

## Final Design & Results

The surface texture issue was first confirmed by hand-sanding the prototype — beetles gripped the roughened surface successfully. For the final print, the team used Bambu Lab Studio's **fuzzy skin** setting, which introduces small random variations to the outer wall during printing, creating a consistent grip texture without any post-processing.

### Final Design Test Results

| Requirement | Test Method | Result |
|---|---|---|
| Size (max 10″ × 4″) | Ruler measurement | ✅ Pass |
| Cleanability | Assemble/disassemble; wash with water | ✅ Pass |
| Surface Texture | Place real bugs on design and observe | ✅ Pass |
| Elevation | Place real bugs on design and observe climbing | ✅ Pass |
| Ventilation | Dry-ice fog test | ✅ Pass |
| Strength | Drop test | ✅ Pass |

**All 6 tests passed on the final version.**

---

## Challenges & Lessons Learned

The primary challenge was surface texture — the team initially focused on overall shape and internal geometry without considering how beetles physically interact with surfaces. The smooth PLA print caused both the texture and elevation test failures. The fix was straightforward once identified, but earlier usability thinking about insect locomotion would have caught it before testing.

**Key takeaway:** Test with actual animals or analogous conditions as early as possible, not just against geometric requirements.

---

## Future Improvements

- Experiment with different fuzzy skin intensities or alternative texturing methods to further optimize grip
- Adjust internal tunnel dimensions and chamber sizes to better match observed burrowing and dog-piling behaviors
- Explore scaling the design to a larger variant (12–15″ long) for the large habitat bins
- Add a bark-like surface pattern to the exterior for a more convincing natural appearance