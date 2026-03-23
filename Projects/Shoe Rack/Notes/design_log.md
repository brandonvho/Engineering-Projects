# Design Log – Shoe Rack Project  

---

## Date: January 19, 2026  

### Work Completed  
- Developed initial prototype design and assembly model for the shoe rack.  
- Practiced part modeling and assembly workflows in Autodesk Fusion.  

### Notes  
- Focused on improving proficiency with parametric modeling, constraints, and component assembly.  

### Next Steps  
- 3D print connector prototypes.  

---

## Date: January 19, 2026  

### Work Completed  
- Designed and printed a separate peg-and-hole connection prototype prior to integrating it into the final connector component.  

### Notes  
- 27 g of PLA filament used for test prints.  

### Next Steps  
- Test fit printed components.  
- Integrate connection system into final connector piece.  
- Print and test fit with wood components.  

---

## Date: January 19, 2026  

### Evaluation of Version 1 Connection  

Testing revealed that the initial peg-and-hole design did not perform as intended.  

- The peg head was overly thick, limiting elastic deformation.  
- Measurement with calipers showed only a 0.01 in diameter reduction when compressed.  
- The mating hole was oversized, resulting in noticeable shaft free play after insertion.  

The Version 1 design was unnecessarily complex and better suited for rotational joints rather than fixed structural connections. Based on this evaluation, I transitioned to a simplified cylindrical interference-fit design inspired by interlocking brick systems.  

### Design Considerations  
- The peg will primarily experience tension and compression forces rather than shear.  
- Internal infill structure will support applied loads.  
- Print orientation is not critical for structural integrity in this application.  
- Circular holes printed on the YZ plane may exhibit sagging; simplified cylindrical geometry mitigates dimensional distortion risks.  

25 g of PLA filament was used to print Version 2 test components.  

### Design Changes (Version 2)  
- Removed retaining lip; replaced with a straight cylindrical peg.  
- Peg and hole designed to equal nominal diameters.  
- Added chamfers to both peg and hole to improve assembly alignment and insertion.  
- Material: Sunlu PLA.  
- Printer: Bambu Lab P1S.  
- Modified print settings:  
  - Wall loops increased from 2 → 4 to improve outer wall strength.  
  - Sparse infill pattern changed from Grid → Gyroid for more uniform multidirectional strength.  

### Next Steps  
- Evaluate fit and retention strength of Version 2 connection.  
- Integrate updated connection geometry into shoe rack bracket.  

---

## Date: January 19, 2026  

### Evaluation of Version 2 Prototype  

- Peg-and-hole connection achieved a snug fit.  
- Components were easy to assemble and required moderate force to disassemble, which is acceptable for the intended application.  
- Printed using default strength settings.  
- Total material usage: approximately 300 g PLA (including supports).  
- Total print time: 9 hours.  

### Work Completed  
- Integrated Version 2 peg-and-hole geometry into shoe rack bracket.  
- Reduced chamfer size from 0.10 in → 0.05 in for improved fit tolerance.  

---

## Date: January 20, 2026  

### Evaluation  

- Printed Version 1 of the shoe rack bracket.  
- Print time (9 hours) and filament usage (300 g) were determined to be inefficient for large-scale production.  
- A multi-tier shelf would require four brackets per tier.  
- Printer limitations allow only one bracket per print cycle, making production time excessive.  

### Next Steps  
- Redesign bracket to reduce material usage and print time.  
- Optimize geometry to allow multiple brackets per single print session.  

---

## Date: February 13, 2026  

### Measurements and Calibration  

- Purchased 1 in × 10 in × 10 ft common board.  
- Measured actual thickness: 0.76 in – 0.78 in (nominal lumber dimension deviation noted).  

- Printed 20 mm × 20 mm × 20 mm calibration cube.  
- Measured dimensional variance:  
  - X and Y axes print approximately 0.2 mm undersized (~0.0079 in).  
- Determined deviation to be negligible for current project tolerances.  

### Next Steps  
- Cut common board into 5 in segments.  
- Design and print Shoe Rack Bracket Version 2 with dimensional adjustments as needed.  

---

## Date: February 13, 2026

### Work Completed
- Designed and test printed Shoe Rack Bracket V2.  
- Optimized geometry to reduce material usage and manufacturing time.  
- Reduced material consumption to approximately 50g (from previous 300g).  
- Decreased total print time to 2.5 hours.
- New version still uses 4 pieces.

---

## Date: February 15, 2026

### Note: 
- shoe rack bracket v2 fits tightly onto the wood. Requires firm pressure or rubber mallet. If bracket breaks or cracks i might change the spacing to 0.77 -> 0.78 inch. Current print uses 2 layer wall and 15% infill. If more strength is needed, those settings can be adjusted. The current design has trapezoids on the bottom and top has a peg and hole to keep the legs in place. It is enough to keep them from sliding especially when the wooden boards have weight. However, the wood is warped and causes the legs to sometimes be at an angle. I have decided to change the trapezoid shape to a square peg and hole. The trapezoid is more aesthetically pleasing, so i might adjust the shape and height to fit better when the legs are not perfectly aligned. 

---

## Date: February 16, 2026

### Note:
- I created transition pieces to allow v2.3 to stack on v2.0 so i dont waste material. V2.3 is the final product so far. It holds the frames together despite the wood being warped.