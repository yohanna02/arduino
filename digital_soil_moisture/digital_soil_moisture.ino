#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SOIL_MOISTURE_PIN A0
#define PUMP_PIN 5

#define WATER_LEVEL_ON_PERCENT 40
#define WATER_LEVEL_OFF_PERCENT 85

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();

  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("construction"));
  delay(3000);
  
  lcd.clear();
  lcd.print(F("of a digital"));
  lcd.setCursor(0, 1);
  lcd.print(F(" soil moisture sensor"));
  delay(3000);
  
  lcd.clear();
  lcd.print(F("Anaja Faith Ejuile"));
  lcd.setCursor(0, 1);
  lcd.print(F("17/48932U/2"));
  delay(3000);
  
  lcd.clear();
  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Prof. H.Kabir"));
  delay(3000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  int soilRawValue = analogRead(SOIL_MOISTURE_PIN);
  
  int soilLevel = map(soilRawValue, 0, 1023, 0, 100);

  lcd.setCursor(0, 0);
  lcd.print(F("W Level:"));
  lcd.print(soilLevel);
  lcd.print(F("%   "));

  lcd.setCursor(0, 1);
  if (soilLevel >= WATER_LEVEL_ON_PERCENT && soilLevel <= WATER_LEVEL_OFF_PERCENT) {
    digitalWrite(PUMP_PIN, HIGH);
    lcd.print(F("Pump ON    "));
  }
  else {
    digitalWrite(PUMP_PIN, LOW);
    lcd.print(F("Pump OFF   "));
  }
}
