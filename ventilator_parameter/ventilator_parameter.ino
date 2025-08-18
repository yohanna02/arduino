#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // LCD with I2C address 0x27

unsigned long lastScrollTime = 0;
int scrollIndex = 0;
const int scrollDelay = 3000;  // 3 seconds between scrolls

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
  if (millis() - lastScrollTime > scrollDelay) {
    lastScrollTime = millis();
    showNextPage(scrollIndex);
    scrollIndex = (scrollIndex + 1) % 2;  // Now only 2 pages
  }
}

void showNextPage(int index) {
  lcd.clear();
  switch (index) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Resp Rate:   22 BPM");
      lcd.setCursor(0, 1);
      lcd.print("Tidal Vol: 118.7mL");
      lcd.setCursor(0, 2);
      lcd.print("Peak Press: 21.6cm");
      break;

    case 1:
      lcd.setCursor(0, 0);
      lcd.print("I:E Ratio:   1:2.02");
      lcd.setCursor(0, 1);
      lcd.print("All Params Stable");
      lcd.setCursor(0, 2);
      lcd.print("Deviation < 3%");
      lcd.setCursor(0, 3);
      lcd.print("Mode: VENTILATOR");
      break;
  }
}
