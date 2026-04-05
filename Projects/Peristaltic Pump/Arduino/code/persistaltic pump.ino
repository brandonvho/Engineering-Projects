#include <LiquidCrystal.h>   // Library to control standard LCDs
#include <Stepper.h>         // Library to control stepper motors
#include <Keypad.h>          // Library to read matrix keypads

// --- Stepper Configuration ---
const int STEPS_PER_REV = 2048; // Number of steps per full rotation (depends on your stepper)
const float ML_PER_REV = 1.2;   // How many mL your pump dispenses per one full rotation
                                 // You must calibrate this for your system

// Initialize stepper motor
// Pins 8, 10, 9, 11 are connected to IN1-IN4 of ULN2003 driver board
Stepper myStepper(STEPS_PER_REV, 8, 10, 9, 11);

// --- LCD Setup ---
// RS, EN, D4, D5, D6, D7 pins on Arduino
LiquidCrystal lcd(12, 7, 5, 4, 3, 2);

// --- Keypad Setup ---
// Define keypad size
const byte ROWS = 4; 
const byte COLS = 4; 

// Define the keymap (what each key outputs)
char keys[ROWS][COLS] = {
  {'1','2','3','A'}, 
  {'4','5','6','B'},
  {'7','8','9','C'}, 
  {'*','0','#','D'}
};

// Arduino pins connected to rows and columns of keypad
byte rowPins[ROWS] = {22, 23, 24, 25}; 
byte colPins[COLS] = {26, 27, 28, 29}; 

// Initialize the keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- Variables ---
String inputVolume = ""; // Stores the volume entered by the user as a string

void setup() {
  lcd.begin(16, 2);       // Initialize LCD with 16 columns, 2 rows
  lcd.print("Enter Volume:"); // Display initial prompt
  myStepper.setSpeed(10); // Set stepper speed in RPM (adjust as needed)
}

void loop() {
  char key = keypad.getKey(); // Read a key press from the keypad

  if (key) { // If a key is pressed
    if (key >= '0' && key <= '9') { 
      // If the key is a number, append it to inputVolume
      inputVolume += key;

      // Display the updated number on the LCD
      lcd.setCursor(0, 1);         // Move to second line
      lcd.print("                "); // Clear previous input (16 spaces)
      lcd.setCursor(0, 1);         // Move cursor back to start
      lcd.print(inputVolume);      // Print new input

    } else if (key == '*') { 
      // If '*' is pressed, clear the current input
      inputVolume = "";
      lcd.setCursor(0, 1);
      lcd.print("                "); // Clear second line

    } else if (key == '#') { 
      // If '#' is pressed, confirm input and start dispensing
      if (inputVolume.length() > 0) { // Only proceed if something was entered
        float volume = inputVolume.toFloat(); // Convert string to float
        // Calculate how many stepper steps are needed
        long stepsToRun = (volume / ML_PER_REV) * STEPS_PER_REV;

        // Notify user that dispensing is starting
        lcd.clear();
        lcd.print("Dispensing...");

        // Run stepper motor
        myStepper.step(stepsToRun);

        // Notify user that dispensing is done
        lcd.clear();
        lcd.print("Done!");
        delay(2000); // Pause for 2 seconds to allow user to read

        // Reset for next input
        inputVolume = "";
        lcd.clear();
        lcd.print("Enter Volume:"); // Prompt for next input
      }
    }
  }
}