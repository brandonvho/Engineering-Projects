#include <LiquidCrystal.h>
#include <Stepper.h>
#include <Keypad.h>

// ── Pump calibration ────────────────────────────────────────────────
const int   STEPS_PER_REV  = 2048;   // 28BYJ-48 full-step count
const float ML_PER_REV     = 1.2;    // mL per revolution — CALIBRATE THIS
const int   MOTOR_RPM      = 12;     // Tune for your tubing/roller geometry
const float MAX_VOLUME_ML  = 999.9;  // Safety ceiling
const byte  MAX_INPUT_LEN  = 5;      // e.g. "999.9"

// ── Pin assignments ──────────────────────────────────────────────────
// Stepper: IN1-IN4 of ULN2003 → Arduino pins (interleaved for 28BYJ-48)
Stepper myStepper(STEPS_PER_REV, 8, 10, 9, 11);

// LCD: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(12, 7, 5, 4, 3, 2);

// Keypad
const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {22, 23, 24, 25};
byte colPins[COLS] = {26, 27, 28, 29};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ── State ────────────────────────────────────────────────────────────
enum State { ENTERING, DISPENSING };
State state = ENTERING;

String  inputVolume   = "";
bool    hasDecimal    = false;
long    stepsRemaining = 0;
long    totalSteps    = 0;

// ── Helpers ──────────────────────────────────────────────────────────

// Print a string left-padded to 16 chars to avoid flicker
void lcdPrintLine(byte row, String text) {
  lcd.setCursor(0, row);
  while (text.length() < 16) text += ' ';
  lcd.print(text);
}

void showEnterPrompt() {
  lcdPrintLine(0, "Volume (mL):");
  lcdPrintLine(1, inputVolume.length() ? inputVolume : "_");
}

// ── Setup ────────────────────────────────────────────────────────────
void setup() {
  lcd.begin(16, 2);
  myStepper.setSpeed(MOTOR_RPM);
  showEnterPrompt();
}

// ── Main loop ────────────────────────────────────────────────────────
void loop() {
  if (state == DISPENSING) {
    runOneStep();
    return;               // Keep keypad responsive during dispense
  }

  char key = keypad.getKey();
  if (!key) return;

  // ── Digit ─────────────────────────────────────────────────────────
  if (key >= '0' && key <= '9') {
    if (inputVolume.length() < MAX_INPUT_LEN) {
      // Prevent leading zeros (except "0.")
      if (!(inputVolume == "0" && key == '0')) {
        inputVolume += key;
        showEnterPrompt();
      }
    }

  // ── Decimal point (A key) ─────────────────────────────────────────
  } else if (key == 'A') {
    if (!hasDecimal && inputVolume.length() < MAX_INPUT_LEN - 1) {
      if (inputVolume.length() == 0) inputVolume = "0";
      inputVolume += '.';
      hasDecimal = true;
      showEnterPrompt();
    }

  // ── Clear / Cancel ────────────────────────────────────────────────
  } else if (key == '*') {
    if (state == DISPENSING) {
      // Emergency stop
      stepsRemaining = 0;
    } else {
      inputVolume = "";
      hasDecimal  = false;
      showEnterPrompt();
    }

  // ── Confirm ───────────────────────────────────────────────────────
  } else if (key == '#') {
    if (inputVolume.length() == 0 || inputVolume == "0") return;

    float volume = inputVolume.toFloat();

    if (volume <= 0 || volume > MAX_VOLUME_ML) {
      lcdPrintLine(0, "Bad volume!");
      lcdPrintLine(1, "Max: 999.9 mL");
      delay(1500);
      inputVolume = "";
      hasDecimal  = false;
      showEnterPrompt();
      return;
    }

    // Start dispensing
    totalSteps     = (long)((volume / ML_PER_REV) * STEPS_PER_REV);
    stepsRemaining = totalSteps;
    state          = DISPENSING;

    lcdPrintLine(0, "Dispensing " + String(volume, 1) + "mL");
    lcdPrintLine(1, "[*] to cancel");
  }
}

// ── Non-blocking single-step dispense ────────────────────────────────
void runOneStep() {
  if (stepsRemaining > 0) {
    myStepper.step(1);
    stepsRemaining--;

    // Update progress every 64 steps to avoid LCD overhead
    if (stepsRemaining % 64 == 0) {
      int pct = (int)(100L * (totalSteps - stepsRemaining) / totalSteps);
      lcdPrintLine(1, "Progress: " + String(pct) + "%  ");
    }
  } else {
    // Done
    state      = ENTERING;
    inputVolume = "";
    hasDecimal  = false;

    bool cancelled = (stepsRemaining == 0 && totalSteps > 0 &&
                      keypad.getKey() == '*');
    lcdPrintLine(0, cancelled ? "Cancelled." : "Done!");
    lcdPrintLine(1, "");
    delay(1500);
    showEnterPrompt();
  }
}