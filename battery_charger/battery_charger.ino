#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define VOLTAGE_SENSOR A0

void setup() {
  // put your setup code here, to run once:
  pinMode(VOLTAGE_SENSOR, INPUT);
  lcd.begin(16, 2);

  /* Name: Abdulrahman Aliyu
Reg number: 19/05/05/037
Supervisor name: Engr. Dr. Adam Bukar
Project Title: Design and Construction of 12volt car battery automatic charger with voltage level indicator */

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("Construction"));
  delay(3000);
  lcd.clear();

  lcd.print(F("of 12volt car"));
  lcd.setCursor(0, 1);
  lcd.print(F("battery automatic"));
  delay(3000);
  lcd.clear();

  lcd.print(F("charger with"));
  lcd.setCursor(0, 1);
  lcd.print(F("voltage level"));
  delay(3000);
  lcd.clear();

  lcd.print(F("indicator"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Abdulrahman Aliyu"));
  lcd.setCursor(0, 1);
  lcd.print(F("19/05/05/037"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr.Dr.Adam Bukar"));
  delay(3000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0, 0);
  lcd.print(F("Battery Volage"));
  lcd.setCursor(0, 1);
  lcd.print(readVoltage(VOLTAGE_SENSOR));
  lcd.print(F("V    "));
  delay(2000);
}

float readVoltage(int pin) {
  float R1 = 30000.0;
  float R2 = 7500.0;
  float ref_voltage = 5.0;
  int adc_value = analogRead(pin);

  // Determine voltage at ADC input
  float adc_voltage = (adc_value * ref_voltage) / 1024.0;

  // Calculate voltage at divider input
  float in_voltage = adc_voltage * (R1 + R2) / R2;

  return in_voltage;
}
