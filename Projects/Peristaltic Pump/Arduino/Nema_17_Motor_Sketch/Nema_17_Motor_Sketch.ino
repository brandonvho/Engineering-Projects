#include <LiquidCrystal.h>   // Controls the 16x2 LCD display
#include <AccelStepper.h>    // Non-blocking stepper control with speed/acceleration
#include <Keypad.h>          // Reads key presses from the 4x4 membrane keypad

// ══════════════════════════════════════════════════════════════════════
// PUMP CALIBRATION
// These are the two values you will need to measure and adjust once
// the physical pump body is assembled and tubing is installed.
// ══════════════════════════════════════════════════════════════════════

const int   STEPS_PER_REV = 1600;  // Number of steps for one full rotation of the
                                    // NEMA 17 stepper motor at 1/8 microstepping
                                    // via the A4988 driver (200 full steps × 8).
                                    // Adjust if you change the MS1/MS2/MS3 jumper
                                    // settings on the A4988.

const float ML_PER_REV    = 1.0;   // How many mL of liquid the pump dispenses per
                                    // one full motor revolution. This must be
                                    // calibrated empirically: run a known number of
                                    // revolutions, measure the displaced volume with
                                    // a syringe, then divide volume by revolutions.

const float MAX_VOLUME_ML = 999.9; // Maximum volume the user is allowed to enter (mL).
                                    // Prevents accidental extremely long motor runs.

const byte  MAX_INPUT_LEN = 5;     // Maximum number of characters in the volume input
                                    // field (e.g. "999.9" = 5 chars including the dot).

// ══════════════════════════════════════════════════════════════════════
// COMPONENT SETUP
// ══════════════════════════════════════════════════════════════════════

// Stepper motor driver
// DRIVER mode sends STEP and DIR signals to the A4988 driver board.
// The A4988 handles all coil sequencing internally.
//   STEP → D8,  DIR → D9
// RESET and SLEEP pins on the A4988 must be bridged together.
// MS1 and MS2 tied to 5V, MS3 to GND for 1/8 microstepping.
// ENABLE tied to GND to keep the motor always enabled.
// A 100µF capacitor must be placed across VMOT and GND on the A4988.
AccelStepper myStepper(AccelStepper::DRIVER, 8, 9);

// LCD1602 display
// Arguments: RS, Enable, D4, D5, D6, D7
// RW pin (pin 5 on LCD) must be tied to GND for write-only operation —
// leaving it floating causes a blank display even if all other wiring is correct.
// Backlight anode (pin 15) requires a 220Ω current-limiting resistor to 5V.
// Contrast (pin 3 / V0) is set via a 10kΩ potentiometer wiper between 5V and GND.
LiquidCrystal lcd(12, 7, 5, 4, 3, 2);

// 4x4 membrane keypad
// Physical layout:
//   [ 1 ][ 2 ][ 3 ][ A ]  ← A = decimal point
//   [ 4 ][ 5 ][ 6 ][ B ]  ← B unused
//   [ 7 ][ 8 ][ 9 ][ C ]  ← C unused
//   [ * ][ 0 ][ # ][ D ]  ← * = clear/stop,  # = confirm/run,  D unused
const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {22, 23, 24, 25};  // Arduino pins connected to keypad rows
byte colPins[COLS] = {26, 27, 28, 29};  // Arduino pins connected to keypad columns
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ══════════════════════════════════════════════════════════════════════
// STATE MACHINE
// The program runs in one of two states at any time:
//   ENTERING   — waiting for the user to type a volume on the keypad
//   DISPENSING — motor is running, keypad only checks for cancel (*)
// ══════════════════════════════════════════════════════════════════════
enum State { ENTERING, DISPENSING };
State state = ENTERING;

// ── Runtime variables ─────────────────────────────────────────────────
String inputVolume = "";    // Accumulates the keypad digits as a string (e.g. "12.5")
bool   hasDecimal  = false; // Tracks whether a decimal point has already been entered
                             // so a second '.' keypress is ignored
long   totalSteps  = 0;     // Total steps calculated for the current dispense run,
                             // used to compute the progress percentage

// ══════════════════════════════════════════════════════════════════════
// HELPER FUNCTIONS
// ══════════════════════════════════════════════════════════════════════

// Prints a string on the specified LCD row (0 = top, 1 = bottom).
// Pads with trailing spaces to exactly 16 characters, which overwrites any
// leftover characters from a previous longer string without calling lcd.clear()
// (which would cause visible flicker).
void lcdPrintLine(byte row, String text) {
  lcd.setCursor(0, row);
  while (text.length() < 16) text += ' ';
  lcd.print(text);
}

// Displays the volume entry prompt on the LCD.
// Top line: static label.  Bottom line: current input, or "_" if empty.
void showEnterPrompt() {
  lcdPrintLine(0, "Volume (mL):");
  lcdPrintLine(1, inputVolume.length() ? inputVolume : "_");
}

// ══════════════════════════════════════════════════════════════════════
// SETUP — runs once on power-on or reset
// ══════════════════════════════════════════════════════════════════════
void setup() {
  lcd.begin(16, 2);              // Initialise LCD as 16 columns, 2 rows

  myStepper.setMaxSpeed(3000);   // Hard ceiling on step rate (steps/sec).
                                  // The A4988 + NEMA 17 can handle much higher rates
                                  // than the old 28BYJ-48; tune down if stalling occurs.

  showEnterPrompt();             // Show the initial volume entry screen
}

// ══════════════════════════════════════════════════════════════════════
// MAIN LOOP — runs repeatedly
// ══════════════════════════════════════════════════════════════════════
void loop() {

  // ── DISPENSING state ──────────────────────────────────────────────
  // While the motor is running, skip normal keypad handling and only
  // check for an emergency stop (*). Then advance the motor one step.
  if (state == DISPENSING) {
    char key = keypad.getKey();
    if (key == '*') {
      // Emergency stop: sets the motor's current position as its target,
      // making distanceToGo() immediately return 0 and halting the run.
      // myStepper.stop() is NOT used here because it decelerates gradually,
      // which means distanceToGo() stays non-zero and the done branch
      // in runOneStep() never triggers.
      myStepper.setCurrentPosition(myStepper.currentPosition());
    }
    runOneStep();
    return; // Skip the rest of loop() — no normal keypad input while dispensing
  }

  // ── ENTERING state ────────────────────────────────────────────────
  char key = keypad.getKey();
  if (!key) return; // No key pressed this cycle, nothing to do

  // ── Digit keys (0–9) ──────────────────────────────────────────────
  if (key >= '0' && key <= '9') {
    if (inputVolume.length() < MAX_INPUT_LEN) {
      // Prevent multiple leading zeros (e.g. "00" is not valid)
      // but allow "0." as a valid start for values less than 1 mL
      if (!(inputVolume == "0" && key == '0')) {
        inputVolume += key;
        showEnterPrompt();
      }
    }
    // Silently ignore input beyond MAX_INPUT_LEN characters

  // ── A key — decimal point ─────────────────────────────────────────
  } else if (key == 'A') {
    // Only insert a decimal if one hasn't been added yet, and there is
    // still room for at least one digit after it
    if (!hasDecimal && inputVolume.length() < MAX_INPUT_LEN - 1) {
      if (inputVolume.length() == 0) inputVolume = "0"; // Prefix bare "." with "0"
      inputVolume += '.';
      hasDecimal = true;
      showEnterPrompt();
    }

  // ── * key — clear input ───────────────────────────────────────────
  } else if (key == '*') {
    inputVolume = "";
    hasDecimal  = false;
    showEnterPrompt();

  // ── # key — confirm and start dispensing ─────────────────────────
  } else if (key == '#') {
    // Reject empty input or a bare zero
    if (inputVolume.length() == 0 || inputVolume == "0") return;

    float volume = inputVolume.toFloat();

    // Validate range — reject zero, negative, or over the safety ceiling
    if (volume <= 0 || volume > MAX_VOLUME_ML) {
      lcdPrintLine(0, "Bad volume!");
      lcdPrintLine(1, "Max: 999.9 mL");
      delay(1500);
      inputVolume = "";
      hasDecimal  = false;
      showEnterPrompt();
      return;
    }

    // Calculate how many motor steps are needed to dispense the requested volume.
    // Formula: (volume / mL_per_rev) gives the number of revolutions needed,
    // multiplied by steps_per_rev to convert to steps.
    totalSteps = (long)((volume / ML_PER_REV) * STEPS_PER_REV);

    myStepper.move(totalSteps);  // Queue the target position (relative move)
    myStepper.setSpeed(1400);    // Set constant run speed (steps/sec).
                                  // runSpeedToPosition() uses this value directly
                                  // and ignores any acceleration ramp.
                                  // Tune this value after calibration — the NEMA 17
                                  // can run significantly faster than the old 28BYJ-48.
    state = DISPENSING;          // Switch state machine to dispensing mode

    lcdPrintLine(0, String(volume, 1) + " mL");  // e.g. "5.0 mL"
    lcdPrintLine(1, "Running [*]=stop");
  }
}

// ══════════════════════════════════════════════════════════════════════
// NON-BLOCKING STEP FUNCTION
// Called once per loop() tick while in DISPENSING state.
// Advances the motor by one step if it's time to do so, and updates
// the progress display. When the target is reached (or the run is
// cancelled) it resets the state machine back to ENTERING.
// ══════════════════════════════════════════════════════════════════════
void runOneStep() {
  if (myStepper.distanceToGo() != 0) {
    // runSpeedToPosition() moves toward the target at the fixed speed set
    // by setSpeed(). Unlike run(), it ignores acceleration, giving a smooth
    // constant-speed dispense with no ramp up or ramp down.
    myStepper.runSpeedToPosition();

    // Update the progress percentage on the LCD.
    // Only recalculate every 64 steps to avoid spending too much time on
    // LCD writes, which are slow and would otherwise slow the step rate.
    long remaining = abs(myStepper.distanceToGo());
    if (remaining % 64 == 0) {
      int pct = (int)(100L * (totalSteps - remaining) / totalSteps);
      lcdPrintLine(1, "Progress: " + String(pct) + "%  ");
    }

  } else {
    // distanceToGo() == 0 means either the run completed normally,
    // or the emergency stop zeroed out the target position.
    state      = ENTERING;
    inputVolume = "";
    hasDecimal  = false;
    lcdPrintLine(0, "Done!");
    lcdPrintLine(1, "");
    delay(1500);          // Pause so the user can read the done message
    showEnterPrompt();    // Return to the volume entry screen
  }
}
