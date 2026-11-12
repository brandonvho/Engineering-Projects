/*
  Teeter - self-balancing robot
  Arduino port of Simulation/balance_sim_v5.py

  HARDWARE THIS IS WRITTEN FOR
    * ELEGOO Mega 2560 R3
    * 2x NEMA 17 stepper (17HS4401, 1.8 deg = 200 steps/rev)
    * 2x A4988 driver module
    * MPU-6050 (GY-521 board)
    * 3S LiPo -> A4988 VMOT, and -> LM2596 buck converter (set to 7.5 V) -> Mega Vin

  WIRING (same as Electronics/wiring.md)
    MPU-6050   VCC -> 5V    GND -> GND    SDA -> pin 20    SCL -> pin 21
    Left  A4988   STEP -> pin 3   DIR -> pin 4
    Right A4988   STEP -> pin 5   DIR -> pin 6
    Both  A4988   ENABLE -> pin 7      VDD -> 5V      GND -> GND
                  RESET and SLEEP joined together (on each driver)
                  MS1 -> 5V, MS2 -> 5V, MS3 -> not connected  (= 1/8 microstepping)
                  VMOT / GND -> battery, with a 100 uF capacitor across them
    All grounds joined. Never plug or unplug a motor while the driver has power.

  THE CONTROL LAW (same as the simulation, plus two additions)

      error = tilt - target_tilt                    (radians)
      accel = KP*error + KI*integral + KD*tilt_rate (wheel acceleration, m/s^2)

  What is different from the Python sim:
    * read_imu()  -> a real MPU-6050 (gyro and accelerometer blended together)
    * the physics -> the real robot. The code turns "accel" into stepper
                     pulses: accel -> wheel speed -> steps per second.
    * D term      -> uses the gyro's tilt rate directly instead of
                     (error - prev_error)/dt. Same quantity, far less noise.
    * SPEED LOOP  -> NEW, and required. target_tilt is no longer fixed at
                     upright: the faster the robot rolls, the further back it
                     aims to lean (KV below). Without this the robot leans,
                     accelerates, and runs away until the motors hit top
                     speed.
    * ACCEL FIX   -> NEW, and required. An accelerometer cannot tell "leaning
                     while accelerating" from "upright": driving the wheels
                     forward makes it read as if the robot were leaning back.
                     The code knows how hard it is accelerating the wheels, so
                     it adds that error back on. The sim's ideal sensor hid
                     this problem.
    * safety      -> motors switch off if the robot falls past 45 degrees.

  No libraries to install: it uses only Wire, which comes with the Arduino IDE.

  STATUS: this has been compiled and run on a PC against a simulated robot
  (stand-in Arduino headers), where it balances and recovers from shoves. It
  has NOT been compiled with the Arduino IDE or run on real hardware.
  Follow the first-power-up checklist at the bottom of this file.
*/

#include <Wire.h>

// ====================================================================
//  SECTION 1 - MEASURE THESE ON THE ROBOT
// ====================================================================
const float WHEEL_DIAMETER_M = 0.080;   // wheel diameter in metres (80 mm) - measure the printed wheel
const float STEPS_PER_REV    = 200.0;   // full steps per revolution (1.8 deg motor = 200)
const float MICROSTEPS       = 8.0;     // must match the MS1/MS2/MS3 wiring (1/8 as wired above)

// ====================================================================
//  SECTION 2 - WIRING (any digital pins work)
// ====================================================================
const uint8_t STEP_LEFT_PIN  = 3;
const uint8_t DIR_LEFT_PIN   = 4;
const uint8_t STEP_RIGHT_PIN = 5;
const uint8_t DIR_RIGHT_PIN  = 6;
const uint8_t ENABLE_PIN     = 7;       // to EN on both drivers. LOW = drivers on, HIGH = motors free

// The two motors face opposite ways, so one must spin "backwards" to drive forward.
// If a wheel turns the wrong way, flip its value here.
const bool INVERT_LEFT  = false;
const bool INVERT_RIGHT = true;

// ====================================================================
//  SECTION 3 - IMU MOUNTING
// ====================================================================
// Assumed mounting: board flat, the X arrow printed on it pointing forward, Z up.
// Leaning FORWARD must read as a POSITIVE angle. If it reads negative, set this to -1.
const float ANGLE_SIGN = 1.0;

// The angle at which the robot actually balances, in degrees. It is rarely
// exactly 0 because the centre of mass is never perfectly above the axle.
// Find it by holding the robot at its balance point and reading the Serial output.
const float BALANCE_ANGLE_DEG = 0.0;

// How much to trust the gyro vs the accelerometer when blending (0.98 = 98% gyro).
const float GYRO_BLEND = 0.98;

// ====================================================================
//  SECTION 4 - MOTOR LIMITS (same meaning as in the sim)
// ====================================================================
const float MAX_ACCEL = 15.0;   // m/s^2  - lower it if the steppers skip or stall
const float MAX_SPEED = 1.5;    // m/s    - lower it if they stall at high speed

// ====================================================================
//  SECTION 5 - GAINS
// ====================================================================
// Tilt PID, same units as the sim: wheel acceleration (m/s^2) per radian.
float KP = 40.0;    // start from the sim values, then tune on the robot
float KI = 5.0;
float KD = 2.4;

// Speed loop: how far to lean back per unit of wheel speed (radians per m/s).
// 0.15 is about 8.6 degrees of lean-back at 1 m/s. Too low: slow to stop after
// a shove and wanders. Too high (around 0.3): fast shaking.
float KV = 0.15;

const float MAX_LEAN_DEG   = 10.0;  // the speed loop may not ask for more lean than this
const float INTEGRAL_LIMIT = 1.0;   // caps the I term so it cannot build up without limit (rad*s)

// Correct the accelerometer for the wheels' own acceleration (see ACCEL FIX above).
// This assumes the IMU sits close to the wheel axle and the wheels do not slip.
const bool COMPENSATE_ACCEL = true;

// ====================================================================
//  SECTION 6 - TIMING AND SAFETY
// ====================================================================
const unsigned long LOOP_US = 5000;        // control loop period: 5000 us = 200 Hz (DT_CTRL in the sim)
const float DT = LOOP_US / 1000000.0;      // the same period in seconds

const float FALL_ANGLE_DEG  = 45.0;        // past this, cut the motors
const float START_ANGLE_DEG = 3.0;         // must be held within this of balance to (re)start

const bool PRINT_DEBUG = true;             // print angle and speed over Serial (115200 baud)

// ====================================================================
//  Everything below should not need changing
// ====================================================================

// ---- derived constants ----
const float PI_F = 3.14159265;
const float STEPS_PER_METRE = (STEPS_PER_REV * MICROSTEPS) / (PI_F * WHEEL_DIAMETER_M);
const float DEG_TO_RAD_F = PI_F / 180.0;

// ---- step pulse generator (Timer1 interrupt, 20 000 times per second) ----
const unsigned int TICK_HZ = 20000;
volatile unsigned int stepsPerSecond = 0;  // set by the control loop, read by the interrupt
volatile unsigned int stepPhase = 0;       // running total that decides when to pulse
volatile uint8_t *stepPortLeft;            // direct access to the STEP pins, looked up in setup()
volatile uint8_t *stepPortRight;           //   (much faster than digitalWrite inside an interrupt)
uint8_t stepMaskLeft;
uint8_t stepMaskRight;

// ---- MPU-6050 ----
const uint8_t MPU_ADDR = 0x68;
const float GYRO_LSB_PER_DEG_S = 65.5;     // +/-500 deg/s range
float gyroBiasY = 0.0;                     // gyro reading when perfectly still (found at start-up)

// ---- controller state (same names as the sim) ----
float angleDeg   = 0.0;    // current tilt estimate, degrees
float integral   = 0.0;    // running total of error
float wheelSpeed = 0.0;    // commanded wheel speed, m/s
float wheelAccel = 0.0;    // wheel acceleration applied during the last loop, m/s^2
bool  balancing  = false;  // are the motors active?
unsigned long nextLoopAt = 0;
uint8_t printCounter = 0;


// Runs 20 000 times a second. Adds the wanted step rate to a running total;
// each time the total passes 20 000, both motors get one step pulse.
ISR(TIMER1_COMPA_vect) {
  stepPhase += stepsPerSecond;
  if (stepPhase >= TICK_HZ) {
    stepPhase -= TICK_HZ;
    *stepPortLeft  |= stepMaskLeft;      // both STEP pins high
    *stepPortRight |= stepMaskRight;
    delayMicroseconds(2);                // the A4988 needs at least 1 us
    *stepPortLeft  &= ~stepMaskLeft;     // both STEP pins low
    *stepPortRight &= ~stepMaskRight;
  }
}

void setupStepTimer() {
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS11);     // CTC mode, prescaler 8 -> 2 MHz timer clock
  OCR1A  = (2000000UL / TICK_HZ) - 1;      // 2 MHz / 20 kHz = 100 counts per tick
  TIMSK1 = (1 << OCIE1A);                  // enable the interrupt
  interrupts();
}

// Turn a wheel speed in m/s into direction pins + a step rate.
void setWheelSpeed(float speed) {
  bool forward = (speed >= 0.0);
  digitalWrite(DIR_LEFT_PIN,  (forward != INVERT_LEFT)  ? HIGH : LOW);
  digitalWrite(DIR_RIGHT_PIN, (forward != INVERT_RIGHT) ? HIGH : LOW);

  float rate = fabs(speed) * STEPS_PER_METRE;        // steps per second
  if (rate > TICK_HZ) rate = TICK_HZ;                // cannot pulse faster than the timer ticks
  unsigned int r = (unsigned int)rate;
  noInterrupts();                                    // update atomically (it is a 2-byte value)
  stepsPerSecond = r;
  interrupts();
}

void mpuWrite(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

// Read one 16-bit value from the IMU: high byte first, then low byte.
int16_t read16() {
  uint8_t high = Wire.read();
  uint8_t low  = Wire.read();
  return (int16_t)(((uint16_t)high << 8) | low);
}

// Read the IMU. Gives the tilt from the accelerometer alone (degrees) and the
// tilt rate from the gyro (degrees per second). Returns false if the read failed.
bool readImu(float &accelAngleDeg, float &gyroRateDegS) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);                                  // first data register (ACCEL_XOUT_H)
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(MPU_ADDR, (uint8_t)14) != 14) return false;

  int16_t ax = read16();                             // accel X
  read16();                                          // accel Y, not needed
  int16_t az = read16();                             // accel Z
  read16();                                          // temperature, not needed
  read16();                                          // gyro X, not needed
  int16_t gy = read16();                             // gyro Y (rotation about the wheel axle)
  read16();                                          // gyro Z, not needed

  // Leaning forward tips the X axis downward, so gravity shows up as negative X.
  accelAngleDeg = ANGLE_SIGN * atan2(-(float)ax, (float)az) * 180.0 / PI_F;
  gyroRateDegS  = ANGLE_SIGN * ((float)gy / GYRO_LSB_PER_DEG_S - gyroBiasY);
  return true;
}

// Average the gyro while the robot is held still, so drift can be subtracted later.
void calibrateGyro() {
  float sum = 0.0;
  int good = 0;
  for (int i = 0; i < 500; i++) {
    float a, g;
    if (readImu(a, g)) { sum += g; good++; }
    delay(2);
  }
  if (good > 0) gyroBiasY = ANGLE_SIGN * (sum / good);   // undo the sign so it is in raw gyro terms
}

void stopBalancing() {
  balancing = false;
  wheelSpeed = 0.0;
  wheelAccel = 0.0;
  integral = 0.0;
  setWheelSpeed(0.0);
  digitalWrite(ENABLE_PIN, HIGH);                    // release the motors
}

void setup() {
  pinMode(STEP_LEFT_PIN, OUTPUT);
  pinMode(STEP_RIGHT_PIN, OUTPUT);
  pinMode(DIR_LEFT_PIN, OUTPUT);
  pinMode(DIR_RIGHT_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH);                    // motors off until it is upright

  // Look up the fast-access register and bit for each STEP pin.
  stepPortLeft  = portOutputRegister(digitalPinToPort(STEP_LEFT_PIN));
  stepMaskLeft  = digitalPinToBitMask(STEP_LEFT_PIN);
  stepPortRight = portOutputRegister(digitalPinToPort(STEP_RIGHT_PIN));
  stepMaskRight = digitalPinToBitMask(STEP_RIGHT_PIN);

  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);                             // fast I2C so a read fits easily in 5 ms

  mpuWrite(0x6B, 0x00);                              // wake the MPU-6050
  mpuWrite(0x1A, 0x03);                              // built-in low-pass filter, ~44 Hz
  mpuWrite(0x1B, 0x08);                              // gyro range +/-500 deg/s
  mpuWrite(0x1C, 0x00);                              // accel range +/-2 g
  delay(100);

  Serial.println(F("Hold the robot still: calibrating gyro..."));
  calibrateGyro();

  float a, g;
  if (readImu(a, g)) angleDeg = a;                   // start the estimate from the accelerometer
  else Serial.println(F("MPU-6050 not responding - check wiring"));

  setupStepTimer();
  Serial.println(F("Ready. Stand the robot upright to start."));
  nextLoopAt = micros() + LOOP_US;
}

void loop() {
  // ---- wait for the next 5 ms tick (same job as "if t >= next_ctrl" in the sim) ----
  if ((long)(micros() - nextLoopAt) < 0) return;
  nextLoopAt += LOOP_US;

  // ---- 1. read the sensor ----
  float accelAngleDeg, gyroRateDegS;
  if (!readImu(accelAngleDeg, gyroRateDegS)) {       // lost the IMU: stop rather than guess
    stopBalancing();
    return;
  }

  // Undo the lean the accelerometer falsely reports while the wheels accelerate.
  if (COMPENSATE_ACCEL) {
    accelAngleDeg += atan2(wheelAccel, 9.81) * 180.0 / PI_F;
  }

  // Blend: the gyro is smooth but drifts, the accelerometer is noisy but never drifts.
  angleDeg = GYRO_BLEND * (angleDeg + gyroRateDegS * DT) + (1.0 - GYRO_BLEND) * accelAngleDeg;

  float tiltFromBalanceDeg = angleDeg - BALANCE_ANGLE_DEG;

  // ---- 2. safety: fallen over, or not yet stood up ----
  if (balancing && fabs(tiltFromBalanceDeg) > FALL_ANGLE_DEG) {
    stopBalancing();
  }
  if (!balancing && fabs(tiltFromBalanceDeg) < START_ANGLE_DEG) {
    balancing = true;
    digitalWrite(ENABLE_PIN, LOW);                   // motors on
  }

  if (balancing) {
    // ---- 3. speed loop: rolling forward -> aim to lean back, and vice versa ----
    float maxLean = MAX_LEAN_DEG * DEG_TO_RAD_F;
    float targetTilt = constrain(-KV * wheelSpeed, -maxLean, maxLean);   // radians

    // ---- 4. the tilt PID (this block is the sim's controller) ----
    float tilt  = tiltFromBalanceDeg * DEG_TO_RAD_F;      // degrees -> radians (the v5 fix)
    float rate  = gyroRateDegS * DEG_TO_RAD_F;            // tilt rate in rad/s, for the D term
    float error = tilt - targetTilt;

    integral += error * DT;
    integral = constrain(integral, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);

    float accel = KP * error + KI * integral + KD * rate; // wheel acceleration, m/s^2
    accel = constrain(accel, -MAX_ACCEL, MAX_ACCEL);

    // ---- 5. acceleration -> speed -> step pulses (replaces the sim's physics) ----
    float previousSpeed = wheelSpeed;
    wheelSpeed += accel * DT;
    wheelSpeed = constrain(wheelSpeed, -MAX_SPEED, MAX_SPEED);
    wheelAccel = (wheelSpeed - previousSpeed) / DT;     // what the wheels will really do this loop
    setWheelSpeed(wheelSpeed);
  }

  // ---- 6. debug output, 20 times a second ----
  if (PRINT_DEBUG && ++printCounter >= 10) {
    printCounter = 0;
    Serial.print(F("angle_deg:"));
    Serial.print(tiltFromBalanceDeg, 2);
    Serial.print(F(" speed_m_s:"));
    Serial.println(wheelSpeed, 3);
  }
}

/*
  FIRST POWER-UP CHECKLIST (do these in order)

  1. Motors unplugged, USB only. Open Serial Monitor at 115200. Tilt the robot
     forward: angle_deg must go POSITIVE. If it goes negative, set ANGLE_SIGN
     to -1. If it barely changes, the IMU is mounted on a different axis than
     assumed.

  2. Hold the robot where it feels balanced in the hand. Whatever angle_deg
     reads there, put that number in BALANCE_ANGLE_DEG and re-upload.

  3. Set the current limit on each A4988 with its trim pot BEFORE connecting
     the motors (17HS4401 is rated 1.7 A; start around 1 A).

  4. Wheels off the ground, battery on. Hold it upright so the motors switch
     on, then tilt forward: both wheels must spin so the robot would drive
     FORWARD (toward the lean). Fix a wrong wheel with INVERT_LEFT /
     INVERT_RIGHT.

  5. On the floor, hand hovering to catch it. Start with KI = 0 and a low KP
     and KD, raise KP until it holds itself up but oscillates, add KD to calm
     it, then add a little KI. Keep KV at 0.15 to begin with. Use the sim
     values as the target, not the starting point.
*/
