#include <LiquidCrystal.h>

LiquidCrystal lcd(A4, A5, A0, A1, A2, A3);

#define D0 9
#define D1 8
#define D2 11
#define D3 10
#define STD 12

#define RELAY1 4
#define RELAY2 5
#define RELAY3 6
#define RELAY4 3

void setup() {
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);
  pinMode(STD, INPUT);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  lcd.begin(16, 2);
  
  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("construction"));
  delay(3000);
  lcd.clear();

  lcd.print(F("of the load"));
  lcd.setCursor(0, 1);
  lcd.print(F("control system"));
  delay(3000);
  lcd.clear();

  lcd.print(F("using Dual Tone"));
  lcd.setCursor(0, 1);
  lcd.print(F("Multi-Frequecy (DTMF)"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Boussine Kanayo"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151721"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Anthony Osise Goodness"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151791"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr Dahiru Zailani Lame"));
  delay(3000);
  lcd.clear();
}

void loop() {
  if (digitalRead(STD) == HIGH) {
    int value = (digitalRead(D3) << 3) | (digitalRead(D2) << 2) | (digitalRead(D1) << 1) | digitalRead(D0);

    lcd.clear();
    switch (value) {
      case 1:
        lcd.print(F("Bulb 1 OFF"));
        digitalWrite(RELAY1, LOW);
        break;
      case 2:
        lcd.print(F("Bulb 1 ON"));
        digitalWrite(RELAY1, HIGH);
        break;
      case 3:
        lcd.print(F("Bulb 2 OFF"));
        digitalWrite(RELAY2, LOW);
        break;
      case 4:
        lcd.print(F("Bulb 2 ON"));
        digitalWrite(RELAY2, HIGH);
        break;
      case 5:
        lcd.print(F("Socket 1 OFF"));
        digitalWrite(RELAY3, LOW);
        break;
      case 6:
        lcd.print(F("Socket 1 ON"));
        digitalWrite(RELAY3, HIGH);
        break;
      case 7:
        lcd.print(F("Socket 2 OFF"));
        digitalWrite(RELAY4, LOW);
        break;
      case 8:
        lcd.print(F("Socket 2 ON"));
        digitalWrite(RELAY4, HIGH);
        break;
      case 9:
        lcd.print(F("ALL ON"));
        digitalWrite(RELAY1, HIGH);
        digitalWrite(RELAY2, HIGH);
        digitalWrite(RELAY3, HIGH);
        digitalWrite(RELAY4, HIGH);
        break;
      case 0:
        lcd.print(F("ALL OFF"));
        digitalWrite(RELAY1, LOW);
        digitalWrite(RELAY2, LOW);
        digitalWrite(RELAY3, LOW);
        digitalWrite(RELAY4, LOW);
        break;
    }

    delay(3000);
    lcd.clear();
  } else {
    lcd.setCursor(0, 0);
    lcd.print(F("Waiting for"));
    lcd.setCursor(0, 1);
    lcd.print(F("command..."));
  }
}
