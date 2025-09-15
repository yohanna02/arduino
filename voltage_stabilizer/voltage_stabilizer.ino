#include <ZMPT101B.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Relay pins
#define RELAY1 1
#define RELAY2 2
#define RELAY3 3
#define RELAY4 4
#define RELAY5 6
#define RELAY6 5

// Voltage sensor pins
#define INPUT_SENSOR_PIN A2
#define OUTPUT_SENSOR_PIN A1

// ZMPT101B sensors
ZMPT101B inputSensor(INPUT_SENSOR_PIN, 50.0);
ZMPT101B outputSensor(OUTPUT_SENSOR_PIN, 50.0);

// --- Relay control (cumulative logic) ---
void setRelay(int level) {
  digitalWrite(RELAY1, (level >= 1) ? HIGH : LOW);
  digitalWrite(RELAY2, (level >= 2) ? HIGH : LOW);
  digitalWrite(RELAY3, (level >= 3) ? HIGH : LOW);
  digitalWrite(RELAY4, (level >= 4) ? HIGH : LOW);
  digitalWrite(RELAY5, (level >= 5) ? HIGH : LOW);
  digitalWrite(RELAY6, (level >= 6) ? HIGH : LOW);
}

// --- Function to get averaged voltage ---
float getAverageVoltage(ZMPT101B &sensor, int samples = 20) {
  float sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += sensor.getRmsVoltage();
    delay(5);  // small delay for stability
  }
  return sum / samples;
}

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.print("Calibrating...");

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  pinMode(RELAY5, OUTPUT);
  pinMode(RELAY6, OUTPUT);

  setRelay(0);  // all off

  inputSensor.setSensitivity(625.0);   // your measured sensitivity
  outputSensor.setSensitivity(595.0);  // your measured sensitivity

  delay(1000);
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
  // Use averaging for smoother readings
  float inputV = getAverageVoltage(inputSensor, 30);
  float outputV = getAverageVoltage(outputSensor, 20);

  // Display values
  lcd.setCursor(0, 0);
  lcd.print("In:");
  lcd.print(inputV, 0);
  lcd.print("V   ");

  lcd.setCursor(0, 1);
  lcd.print("Out:");
  lcd.print(outputV, 0);
  lcd.print("V   ");

  // Relay decision with cumulative logic
  if (inputV >= 80 && inputV < 107) setRelay(1);
  else if (inputV >= 107 && inputV < 137) setRelay(2);
  else if (inputV >= 137 && inputV < 170) setRelay(3);
  else if (inputV >= 170 && inputV < 200) setRelay(4);
  else if (inputV >= 200 && inputV < 250) setRelay(5);
  else if (inputV >= 250) setRelay(6);
  else setRelay(0);  // below 80V

  delay(500);  // update twice a second
}
