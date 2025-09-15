#include <ZMPT101B.h>
#include <LiquidCrystal.h>

// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define INPUT_SENSOR_PIN A5
ZMPT101B inputSensor(INPUT_SENSOR_PIN, 50.0);  // 50Hz mains

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Calibrating...");
  delay(2000);
}

void loop() {
  static float sensitivity = 0.0;
  static bool increasing = true;

  inputSensor.setSensitivity(sensitivity);

  // Measure RMS voltage
  float voltageNow = inputSensor.getRmsVoltage();

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sens:");
  lcd.print(sensitivity, 1);  // one decimal

  lcd.setCursor(0, 1);
  lcd.print("Volt:");
  lcd.print(voltageNow, 1);  // one decimal
  lcd.print("V");

  delay(1000);

  // Sweep sensitivity automatically
  if (increasing) {
    sensitivity += 5.0;  // coarse step
    if (sensitivity > 1000.0) {
      increasing = false;
      sensitivity = 1000.0;
    }
  } else {
    sensitivity -= 5.0;
    if (sensitivity < 0) {
      increasing = true;
      sensitivity = 0.0;
    }
  }
}
