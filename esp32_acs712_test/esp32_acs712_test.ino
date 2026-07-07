#include <LiquidCrystal_I2C.h>
#include "ACS712.h"

ACS712 ACS(34, 3.3, 4095, 185.0);

// initialize the LCD library with I2C address and LCD size
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  ACS.autoMidPoint();
  pinMode(19, OUTPUT);
  digitalWrite(19, HIGH);
  Serial.begin(9600);
  Serial.println("ACS712 current sensor");
  // Initialize the LCD connected
  lcd.init();
  // Turn on the backlight on LCD.
  lcd.backlight();
  lcd.print("ACS712 current");
  lcd.setCursor(0, 1);
  lcd.print("sensor");
  delay(1000);
  lcd.clear();
}

void loop() {
  float Irms = readCurrentWithCheck(ACS);
  lcd.setCursor(0, 0);
  lcd.print(Irms);
  lcd.print(" Amps ");
  delay(100);
}

float readCurrentWithCheck(ACS712 &sensor) {
  const int NUM_READINGS = 100;

  float sum = 0.0;
  for (int i = 0; i < NUM_READINGS; i++) {
    sum += sensor.mA_AC();
    delay(10);
  }

  float mA = sum / NUM_READINGS;
  return mA / 1000.0;
}