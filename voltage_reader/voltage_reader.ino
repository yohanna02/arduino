#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define VOLTAGE_PIN A0

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0, 0);
  lcd.print(F("Volt: "));
  lcd.print(readVoltage(VOLTAGE_PIN));
  lcd.print(F("V    "));
  delay(3000);
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