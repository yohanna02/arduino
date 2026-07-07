#include <Wire.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, A4, A1, A2, A3, A5); // Change 0x27 to 0x3F if display doesn’t show text

const int sensorPin = A0;
const float Vcc = 5.0;         // Arduino supply voltage
const float sensitivity = 0.009; // MPX5010DP datasheet
const float offset = 0.04;

void setup() {
  lcd.begin(16, 2);
  
  lcd.setCursor(0, 0);
  lcd.print("Air pressure");
  lcd.setCursor(0, 1);
  lcd.print(F("machine"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Giwa R.Yerima");
  lcd.setCursor(0, 1);
  lcd.print(F("0038"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Hirnyam E.Goodness");
  lcd.setCursor(0, 1);
  lcd.print(F("0037"));
  delay(3000);
  lcd.clear();
}

void loop() {
  int raw = analogRead(sensorPin);
  float Vout = (raw / 1023.0) * Vcc;

  // Calculate pressure in kPa
  float P_kPa = ((Vout / Vcc) - offset) / sensitivity;
  if (P_kPa < 0) P_kPa = 0;

  // Display pressure
  lcd.setCursor(0, 0);
  lcd.print("Pressure:");
  lcd.setCursor(0, 1);
  lcd.print(P_kPa, 2);
  lcd.print(" kPa     ");

  delay(500);
}
