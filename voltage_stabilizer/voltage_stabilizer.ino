#include <ZMPT101B.h>
#include <LiquidCrystal.h>

// LCD pins (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

// Relay pins
#define RELAY1 1
#define RELAY2 2
#define RELAY3 3
#define RELAY4 4
#define RELAY5 6
#define RELAY6 5

// Voltage sensor pins
#define INPUT_SENSOR_PIN A5
#define OUTPUT_SENSOR_PIN A4

// Expected mains voltage and tolerance
#define ACTUAL_VOLTAGE 220.0f
#define TOLERANCE 3.0f
#define MAX_TOLERANCE_VOLTAGE (ACTUAL_VOLTAGE + TOLERANCE)
#define MIN_TOLERANCE_VOLTAGE (ACTUAL_VOLTAGE - TOLERANCE)

// Calibration steps
#define FAST_STEP 5.0f
#define FINE_STEP 0.25f

// ZMPT101B sensors
ZMPT101B inputSensor(INPUT_SENSOR_PIN, 50.0);
ZMPT101B outputSensor(OUTPUT_SENSOR_PIN, 50.0);

// --- Function to calibrate a ZMPT101B sensor ---
bool calibrateSensor(ZMPT101B &sensor) {
  float sensitivity = 0.0f;
  float voltageNow;

  // Coarse adjustment
  while (sensitivity <= 1000.0f) {
    sensor.setSensitivity(sensitivity);
    voltageNow = sensor.getRmsVoltage();

    if (voltageNow >= MIN_TOLERANCE_VOLTAGE && voltageNow <= MAX_TOLERANCE_VOLTAGE) {
      break;
    }
    sensitivity += FAST_STEP;
  }

  if (sensitivity > 1000.0f) return false;

  // Fine adjustment
  float fineSensitivity = sensitivity - FAST_STEP;
  if (fineSensitivity < 0) fineSensitivity = 0;

  while (fineSensitivity <= sensitivity + FAST_STEP) {
    sensor.setSensitivity(fineSensitivity);
    voltageNow = sensor.getRmsVoltage();

    if (voltageNow >= MIN_TOLERANCE_VOLTAGE && voltageNow <= MAX_TOLERANCE_VOLTAGE) {
      outputSensor.setSensitivity(fineSensitivity);
      return true;
    }
    fineSensitivity += FINE_STEP;
  }

  return false;  // failed to calibrate
}

// --- Relay control (cumulative) ---
void setRelay(int level) {
  // Turn everything off if level = 0
  if (level == 0) {
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY3, LOW);
    digitalWrite(RELAY4, LOW);
    digitalWrite(RELAY5, LOW);
    digitalWrite(RELAY6, LOW);
    return;
  }

  // Otherwise, turn on all relays up to "level"
  digitalWrite(RELAY1, (level >= 1) ? HIGH : LOW);
  digitalWrite(RELAY2, (level >= 2) ? HIGH : LOW);
  digitalWrite(RELAY3, (level >= 3) ? HIGH : LOW);
  digitalWrite(RELAY4, (level >= 4) ? HIGH : LOW);
  digitalWrite(RELAY5, (level >= 5) ? HIGH : LOW);
  digitalWrite(RELAY6, (level >= 6) ? HIGH : LOW);
}

void setup() {
  lcd.begin(16, 2);
  lcd.print("Calibrating...");
  delay(1000);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  pinMode(RELAY5, OUTPUT);
  pinMode(RELAY6, OUTPUT);

  // ensure all off
  setRelay(0);

  // Calibrate Input sensor (only one that has power at startup)
  lcd.clear();
  lcd.print("Input Calib...");
  if (!calibrateSensor(inputSensor)) {
    lcd.clear();
    lcd.print("In Sensor Fail");
    while (true)
      ;  // stop program
  }

  lcd.clear();
  lcd.print("Calib OK");
  delay(1500);
  lcd.clear();

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("construction of"));
  delay(3000);
  lcd.clear();

  lcd.print(F("3.5kVA voltage"));
  lcd.setCursor(0, 1);
  lcd.print(F("stabilizer with"));
  delay(3000);
  lcd.clear();

  lcd.print(F("input 80v to"));
  lcd.setCursor(0, 1);
  lcd.print(F("250v"));
  delay(3000);
  lcd.clear();

  lcd.print(F("output 180v to"));
  lcd.setCursor(0, 1);
  lcd.print(F("240v, by"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Mustapha Sule Yusuf"));
  lcd.setCursor(0, 1);
  lcd.print(F("Gawon Gabriel"));
  delay(3000);
  lcd.clear();
}

void loop() {
  float inputV = inputSensor.getRmsVoltage();
  float outputV = outputSensor.getRmsVoltage();

  // Display
  lcd.setCursor(0, 0);
  lcd.print("In:");
  lcd.print(inputV, 0);
  lcd.print("V ");

  lcd.setCursor(0, 1);
  lcd.print("Out:");
  lcd.print(outputV, 0);
  lcd.print("V ");

  // Relay decision with cumulative logic
  if (inputV >= 80 && inputV < 107) setRelay(1);
  else if (inputV >= 107 && inputV < 137) setRelay(2);
  else if (inputV >= 137 && inputV < 170) setRelay(3);
  else if (inputV >= 170 && inputV < 200) setRelay(4);
  else if (inputV >= 200 && inputV < 250) setRelay(5);
  else if (inputV >= 250) setRelay(6);
  else setRelay(0);  // below 80V

  delay(1000);
}
