# Wakeboard Mount

## Overview
I wanted to hang my wakeboard (Liquid Force Trip 139 cm, with boots) on the wall. I designed two L-shaped brackets that screw into the wall studs and hold the board almost vertical, leaning slightly back against the wall.

## Objectives
- Hold the board and boots securely on the wall with the board leaning slightly back.
- Keep the brackets from sticking out too far from the wall.
- Make the board easy to put up and take down.

## Requirements & Constraints
- Support about 20 lb (board + boots), 10 lb per bracket.
- Screw into studs 32 in apart (every other stud).
- Account for the board's 17.1 in width and 2.6 in rocker. The rocker pushes the board about 0.9 in off the wall at the brackets.
- Printed in PLA, which creeps under constant load and softens around 55–60 °C.

## Design Summary
- L-bracket with a 4.50 in arm, a 0.50 in lip that stops the board's bottom rail, and a 5.00 in tall leg against the wall. This holds the board at about an 8° lean.
- Full-width diagonal brace between the arm and leg to reinforce the corner, with fillets at every joint.
- Two counterbored screw holes above the brace for #8 x 2-1/2" pan-head wood screws with washers.
- Small J-shaped clips above each bracket that hook over the top of the board so it can't tip forward.

## Tools & Technologies
- CAD: SolidWorks
- Manufacturing: Bambu Lab P1S (PLA), printed on its side, 5–6 walls, 40% gyroid infill

## Testing & Results
- First print (3.50 in arm, about 5° lean): the brackets held the board's weight fine, but a light touch made the board fall forward.
- Lengthened the arm to 4.50 in (8° lean) and added top clips. Not yet retested.

## Challenges & Lessons Learned
- My first bracket stuck out too far from the wall. Calculating the arm depth from the board's actual width, rocker, and lean angle let me shrink it.
- Static analysis showed that leaning the board further back barely helps it stay up. Even at 15°, about 2 lb at the top tips it over. The top clips are what actually keep it from falling.
- The brace initially collided with the board's rail and blocked the lower screw, so I shortened it and moved the screws above it.
- I dimensioned the brace thickness vertically instead of perpendicular, which made it much thinner than intended (0.18 in instead of 0.31 in).
- The brace was narrower than the bracket at first, which would have printed as an unsupported overhang. Extruding it full width fixed that.

## Future Improvements
- Print and test the longer arm and the top clips.
- Add felt or rubber pads where the board touches the bracket and clips.
- Reprint in PETG or ASA if the mount will be somewhere hot, like a garage.