#include <LiquidCrystal.h>

LiquidCrystal lcd(4, 5, 6, 7, 8, 9);

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  lcd.print(F("Hello World"));
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
