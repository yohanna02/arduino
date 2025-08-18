#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define soilPin A3
#define pumpPin 6
#define buzzerPin 7

LiquidCrystal_I2C lcd(0x27, 16, 4);

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },  // A = Enter threshold mode
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'G' },
  { 'C', '0', 'E', 'H' }  // C = Clear, E = Enter
};

byte rowPins[ROWS] = { 9, 10, 11, 12 };
byte colPins[COLS] = { 13, A0, A1, A2 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int moistureThreshold = 0;
String input = "";
bool thresholdSet = false;
bool inSetMode = false;

void setup() {
  pinMode(pumpPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(pumpPin, LOW);
  digitalWrite(buzzerPin, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Require user to set threshold at startup
  lcd.setCursor(0, 0);
  lcd.print("Set Moisture %:");
  lcd.setCursor(0, 2);
  lcd.print("Enter + E");
  lcd.setCursor(0, 3);
  lcd.print("C=Clear");

  while (!thresholdSet) {
    handleThresholdInput();
  }

  lcd.clear();
  lcd.print("Threshold set to");
  lcd.setCursor(0, 1);
  lcd.print(moistureThreshold);
  lcd.print("%");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Allow user to enter Set Mode by pressing 'A'
  char key = keypad.getKey();
  if (key == 'A' && !inSetMode) {
    inSetMode = true;
    input = "";
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set Moisture %:");
    lcd.setCursor(0, 2);
    lcd.print("Enter + E");
    lcd.setCursor(0, 3);
    lcd.print("C=Clear A=Exit");
  }

  if (inSetMode) {
    handleThresholdInput();
    return;
  }

  int moistureVal = analogRead(soilPin);
  int moisturePercent = map(moistureVal, 1023, 0, 0, 100);  // 0 = wet, 100 = dry

  lcd.setCursor(0, 0);
  lcd.print("Moisture: ");
  lcd.print(moisturePercent);
  lcd.print("%    ");

  lcd.setCursor(0, 1);
  lcd.print("Threshold: ");
  lcd.print(moistureThreshold);
  lcd.print("%    ");

  lcd.setCursor(0, 2);
  lcd.print("Mode: AUTO Run   ");

  if (moisturePercent < moistureThreshold) {
    digitalWrite(pumpPin, HIGH);
    beepBuzzer();
    lcd.setCursor(0, 3);
    lcd.print("Pump: ON         ");
  } else {
    digitalWrite(pumpPin, LOW);
    digitalWrite(buzzerPin, LOW);
    lcd.setCursor(0, 3);
    lcd.print("Pump: OFF        ");
  }

  delay(500);
}

void handleThresholdInput() {
  char key = keypad.getKey();
  if (key) {
    if (key == 'C') {
      input = "";
      lcd.setCursor(0, 1);
      lcd.print("                ");
    } else if (key == 'E') {
      if (input.length() > 0) {
        int val = input.toInt();
        if (val >= 0 && val <= 100) {
          moistureThreshold = val;
          thresholdSet = true;
          inSetMode = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Threshold set:");
          lcd.setCursor(0, 1);
          lcd.print(val);
          lcd.print("%");
          delay(1500);
          lcd.clear();
        } else {
          lcd.setCursor(0, 1);
          lcd.print("Invalid (0-100%)");
          input = "";
          delay(1000);
          lcd.setCursor(0, 1);
          lcd.print("                ");
        }
      }
    } else if (key == 'A') {
      // Exit Set Mode if already in it
      inSetMode = false;
      input = "";
      lcd.clear();
    } else if (isDigit(key) && input.length() < 3) {
      input += key;
      lcd.setCursor(0, 1);
      lcd.print("Value: ");
      lcd.print(input);
      lcd.print("%      ");
    }
  }
}

void beepBuzzer() {
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
}
