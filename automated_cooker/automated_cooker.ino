#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Keypad.h>

// Pin Definitions
const int oneWireBus = 2;  // Pin for DS18B20 temperature sensor
const int heaterPin = 13;  // Digital pin for heater relay
const int buzzerPin = 8;   // Digital pin for buzzer

// I2C LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DS18B20 Temperature Sensor Setup
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// Keypad Setup
const byte ROWS = 4;  // Four rows
const byte COLS = 3;  // Three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 9, 10, 11, 12 };  // Connect to the row pinouts of the keypad
byte colPins[COLS] = { A0, A1, A2 };     // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables
bool inWarmMode = true;
unsigned long cookingTime = 0;  // Cooking timer in milliseconds
unsigned long startTime = 0;    // Start time of cooking

void setup() {
  // Initialize pins
  pinMode(heaterPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(F("DESIGN AND"));
  lcd.setCursor(0, 1);
  lcd.print(F("CONSTRUCTION OF"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("MICROCONTROLLER"));
  lcd.setCursor(0, 1);
  lcd.print(F("BASED ELECTRIC"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("RICE COOKER"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("MUSTAPHA KYARI ABBA"));
  lcd.setCursor(0, 1);
  lcd.print(F("19/05/05/220"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("ENGR.MUSA ALIYU SARKI"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

  // Initialize Temperature Sensor
  sensors.begin();

  // Initialize System
  digitalWrite(heaterPin, LOW);
  digitalWrite(buzzerPin, LOW);
  lcd.setCursor(0, 1);
  lcd.print("Warm Mode");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Check for user input via keypad
  char key = keypad.getKey();
  if (key) {
    if (key == '*') {
      setCookingTime();
    } else if (key == '#') {
      cancelCookingMode();
    }
  }

  // Warm mode logic
  if (inWarmMode) {
    lcd.setCursor(0, 0);
    lcd.print("Warm Mode   ");
    digitalWrite(heaterPin, LOW);
    delay(1000);
  } else {
    // Cooking mode logic
    lcd.setCursor(0, 0);
    lcd.print("Cooking...  ");

    // Check if cooking time has elapsed
    if (millis() - startTime >= cookingTime) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Time Elapsed");
      buzz();
      delay(2000);
      switchToWarmMode();
    } else {
      digitalWrite(heaterPin, HIGH);  // Keep heater ON during cooking
    }
  }

  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C) {
    lcd.setCursor(0, 1);
    lcd.print(F("Temp: "));
    lcd.print(tempC);
    lcd.print(F("C    "));
  }
}

void setCookingTime() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Time(min):");

  String input = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {
        input += key;
        lcd.setCursor(0, 1);
        lcd.print(input);
        lcd.print(" min   ");
      } else if (key == '#') {  // Confirm input
        if (input.length() > 0) {
          cookingTime = input.toInt() * 60 * 1000;  // Convert minutes to milliseconds
          startTime = millis();
          inWarmMode = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Cooking Time:");
          lcd.setCursor(0, 1);
          lcd.print(input);
          lcd.print(" min");
          delay(2000);
          lcd.clear();
          break;
        }
      } else if (key == '*') {  // Cancel input
        lcd.clear();
        lcd.print("Cancelled");
        delay(2000);
        lcd.clear();
        break;
      }
    }
  }
}

void cancelCookingMode() {
  if (!inWarmMode) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cooking Cancelled");
    delay(2000);
    switchToWarmMode();
  }
}

void switchToWarmMode() {
  inWarmMode = true;
  digitalWrite(heaterPin, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Warm Mode   ");
  delay(1000);
}

void buzz() {
  for (int i = 0; i < 5; i++) {  // Beep 5 times
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    delay(200);
  }
}
