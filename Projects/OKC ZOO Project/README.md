# OKC ZOO Project

## Overview
This project involved designing a habitat enrichment hide for Darkling Beetles at the
Oklahoma City Zoo. The zoo's existing environment — cardboard tubes and egg cartons —
could not be sanitized, lacked structural complexity, and depended on inconsistent
donations. The goal was to create a reusable, natural-looking hide that better supports
the beetles' instinctive behaviors such as burrowing, dog-piling, tunneling, and seeking
dark spaces.

## Objectives
- Develop skills in CAD modeling, rapid prototyping, and iterative design
- Apply a weighted decision matrix to evaluate and select among multiple design concepts
- Design and deliver a functional enrichment hide that meets all client requirements and
  passes a structured testing plan

## Requirements & Constraints
- Must fit within small bins (12" × 6") and large bins (18" × 12"); max 10" long, 4" wide,
  4" tall
- Must be easy to disassemble and clean daily (cleaned every 24 hours)
- Must include ventilation to prevent deadly dog-piling and maintain 68–72°F habitat temp
- Must have multiple entrances and exits (beetles cannot reverse well)
- Must feature a rough/textured surface so beetles can grip and climb
- Must provide dark internal chambers for breeding, egg laying, and stress reduction
- Must appear natural to mimic the beetles' wild environment
- No wood, paint, sealant, stain, or chemicals of any kind
- Must be non-toxic; no staples, plastic tape, or treated materials
- Cost target: $5–$10 in materials

## Design Summary
The final design is a two-piece 3D-printed log hide modeled in Fusion 360. A flat
bottom tray (half-cylinder, 8" × 4") holds leaf litter and substrate for burrowing. A
matching top shell slides onto the tray via a rail-and-groove connection, creating a
fully enclosed log shape when assembled. The interior of the top shell features curved
internal walls that form multiple dark chambers at different elevations. Ventilation slits
run along the right side of the top piece, and several entrance/exit holes line the sides.
When placed in the habitat, roughly 1.5" is buried in substrate, leaving ~2.5" of usable
above-ground height. The final print used Bambu Lab Studio's "fuzzy skin" setting to
generate a rough exterior texture that gives the beetles sufficient grip to climb.

## Tools & Technologies
- **CAD:** Fusion 360 (part modeling and engineering drawings)
- **Electronics (if any):** None
- **Manufacturing:** Bambu Lab 3D printer, brown PLA filament; cardboard and tape for
  rapid prototyping
- **Software:** Bambu Lab Studio (fuzzy skin texture setting for final print)

## Testing & Results
The team tested six requirements on both the initial prototype and the final design:
size, cleanability, surface texture, elevation, ventilation, and structural strength.
The initial prototype passed four of six — it met size limits, disassembled easily,
showed visible airflow through vent slits during a dry-ice fog test, and survived a drop
test. It failed surface texture and elevation because bugs slid off the smooth PLA surface
and could not grip to climb. The final design resolved this by applying the fuzzy skin
print setting, which created a fine random-variation texture across the outer surface.
Bugs placed on the final design successfully gripped and climbed the exterior. All six
tests passed on the final version.

## Challenges & Lessons Learned
The primary challenge was surface texture — the team initially focused on overall shape
and internal geometry without considering how beetles physically interact with surfaces.
The smooth PLA print caused both the texture and elevation test failures. The fix
(hand-sanding confirmed the concept; fuzzy skin made it repeatable) was straightforward
once identified, but earlier usability thinking about insect locomotion would have caught
it before testing. The key lesson: test with actual animals or analogous conditions as
early as possible, not just against geometric requirements.

## Future Improvements
- Experiment with different fuzzy skin intensities or alternative texturing methods to
  further optimize grip for the beetles
- Adjust internal tunnel dimensions and chamber sizes to better match observed
  burrowing and dog-piling behaviors
- Explore scaling the design to a larger variant (12–15" long) for the large habitat bins
- Consider adding a bark-like surface pattern to the exterior for a more convincing
  natural appearance
