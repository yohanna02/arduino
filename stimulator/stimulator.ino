#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define BTN_1 10
#define BTN_2 12

#define PROTOCOL_OUT_ONE 8
#define PROTOCOL_OUT_TWO 9
#define PROTOCOL_OUT_THREE 6
#define PROTOCOL_OUT_FOUR 7

#define STIMULATOR 11

#define MASSAGER_SWITCH 2
#define STIMULATOR_SWITCH 4

int massageLevel = 0;
int stimulationLevel = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);

  pinMode(MASSAGER_SWITCH, INPUT_PULLUP);
  pinMode(STIMULATOR_SWITCH, INPUT_PULLUP);

  pinMode(STIMULATOR, OUTPUT);

  pinMode(PROTOCOL_OUT_ONE, OUTPUT);
  pinMode(PROTOCOL_OUT_TWO, OUTPUT);
  pinMode(PROTOCOL_OUT_THREE, OUTPUT);
  pinMode(PROTOCOL_OUT_FOUR, OUTPUT);

  lcd.init();
  lcd.backlight();

  lcd.print(F("DEVELOPMENT OF"));
  lcd.setCursor(0, 1);
  lcd.print(F("SMART NEUROMUSCULAR"));
  delay(3000);
  lcd.clear();

  lcd.print(F("STIMULATOR EMBEDDED"));
  lcd.setCursor(0, 1);
  lcd.print(F("WITH MASSAGER"));
  delay(3000);
  lcd.clear();

  lcd.print(F("ANIKENE MOSESROCK OFOBUISI"));
  lcd.setCursor(0, 1);
  lcd.print(F("PG/ESUT/2023070000075"));
  delay(3000);
  lcd.clear();

//  lcd.print(F("Supervised by"));
//  lcd.setCursor(0, 1);
//  lcd.print(F("ENGR DR OKOYE J.O"));
//  delay(3000);
//  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  /* if (digitalRead(MASSAGER_SWITCH) && digitalRead(MASSAGER_SWITCH)) {
    lcd.setCursor(0, 0);
    lcd.print(F("Select Mode        "));
    lcd.setCursor(0, 1);
    lcd.print(F("                   "));
    massageLevel = 0;
    stimulationLevel = 0;

    digitalWrite(PROTOCOL_OUT_ONE, LOW);
    digitalWrite(PROTOCOL_OUT_TWO, LOW);
    digitalWrite(PROTOCOL_OUT_THREE, LOW);
    digitalWrite(PROTOCOL_OUT_FOUR, LOW);
  } else */
  if (!digitalRead(MASSAGER_SWITCH)) {
    stimulationLevel = 0;
    lcd.setCursor(0, 0);
    lcd.print(F("Massage Mode    "));
    lcd.setCursor(0, 1);
    lcd.print(massageLevel);
    lcd.print(F("%       "));

    int pwmValue = map(massageLevel, 0, 100, 0, 255);
    analogWrite(STIMULATOR, pwmValue);

    // Turn off all protocol outputs first
    digitalWrite(PROTOCOL_OUT_ONE, LOW);
    digitalWrite(PROTOCOL_OUT_TWO, LOW);
    digitalWrite(PROTOCOL_OUT_THREE, LOW);
    digitalWrite(PROTOCOL_OUT_FOUR, LOW);

    if (!digitalRead(BTN_1)) {
      delay(200);
      massageLevel += 10;
      if (massageLevel > 100) {
        massageLevel = 0;
      }
    } else if (!digitalRead(BTN_2)) {
      delay(200);
      massageLevel -= 10;
      if (massageLevel < 0) {
        massageLevel = 100;
      }
    }
  } else if (digitalRead(MASSAGER_SWITCH)) {
    lcd.setCursor(0, 0);
    lcd.print(F("Stimuation Mode     "));
    lcd.setCursor(0, 1);
    lcd.print(F("Level "));
    lcd.print(stimulationLevel);
    lcd.print(F("      "));

    if (!digitalRead(BTN_1)) {
      delay(200);
      stimulationLevel++;
      if (stimulationLevel > 4) {
        stimulationLevel = 0;
      }
    } else if (!digitalRead(BTN_2)) {
      delay(200);
      stimulationLevel--;
      if (stimulationLevel < 0) {
        stimulationLevel = 4;
      }
    }

    // Activate the correct protocol pin based on stimulationLevel
    switch (stimulationLevel) {
      case 1:
        digitalWrite(PROTOCOL_OUT_TWO, LOW);
        digitalWrite(PROTOCOL_OUT_THREE, LOW);
        digitalWrite(PROTOCOL_OUT_FOUR, LOW);

        digitalWrite(PROTOCOL_OUT_ONE, HIGH);
        break;
      case 2:
        digitalWrite(PROTOCOL_OUT_ONE, LOW);
        digitalWrite(PROTOCOL_OUT_THREE, LOW);
        digitalWrite(PROTOCOL_OUT_FOUR, LOW);

        digitalWrite(PROTOCOL_OUT_TWO, HIGH);
        break;
      case 3:
        digitalWrite(PROTOCOL_OUT_ONE, LOW);
        digitalWrite(PROTOCOL_OUT_TWO, LOW);
        digitalWrite(PROTOCOL_OUT_FOUR, LOW);

        digitalWrite(PROTOCOL_OUT_THREE, HIGH);
        break;
      case 4:
        digitalWrite(PROTOCOL_OUT_ONE, LOW);
        digitalWrite(PROTOCOL_OUT_TWO, LOW);
        digitalWrite(PROTOCOL_OUT_THREE, LOW);

        digitalWrite(PROTOCOL_OUT_FOUR, HIGH);
        break;
      default:
        digitalWrite(PROTOCOL_OUT_ONE, LOW);
        digitalWrite(PROTOCOL_OUT_TWO, LOW);
        digitalWrite(PROTOCOL_OUT_THREE, LOW);
        digitalWrite(PROTOCOL_OUT_FOUR, LOW);
        break;  // Level 0: all outputs remain LOW
    }
  }
}
