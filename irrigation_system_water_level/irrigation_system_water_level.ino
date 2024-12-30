#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions
const int soilMoisturePin = A3;
const int pumpPin = 11;
const int trigPin = 10;
const int echoPin = 9;

// Ultrasonic sensor
const int minWaterLevel = 15;  // Minimum water level in cm

// Keypad setup
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 6, 5, 4, 3 };  // Connect to the row pins
byte colPins[COLS] = { 2, 1, 0 };     // Connect to the column pins
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables
int soilMoistureThreshold = 50;  // Default threshold
String input = "";
bool settingThreshold = true;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(soilMoisturePin, INPUT);
  lcd.setCursor(0, 0);
  lcd.print(F("Moisture threshold"));
}

void loop() {
  // Handle keypad input
  char key = keypad.getKey();

  if (key == '#') {
    settingThreshold = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Moisture threshold"));
    digitalWrite(pumpPin, LOW);
  }

  while (settingThreshold) {
    char key = keypad.getKey();
    if (key == '#') {
      // Confirm setting threshold
      if (input != "") {
        soilMoistureThreshold = input.toInt();
        input = "";
        settingThreshold = false;
        lcd.clear();
        lcd.print(F("Threshold set"));
        delay(3000);
        lcd.clear();
      }
    } else if (key == '*') {
      // Start setting threshold
      input = "";
      settingThreshold = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Moisture threshold"));
    } else if (key >= '0' && key <= '9') {
      // Append key to input
      input += key;
      lcd.setCursor(0, 1);
      lcd.print(input);
    }
  }

  // Measure soil moisture
  int soilMoistureValue = analogRead(soilMoisturePin);
  int soilMoisturePercent = map(soilMoistureValue, 0, 1023, 100, 0);

  // Measure water level
  int waterLevel = measureDistance(trigPin, echoPin);

  // Display readings
  lcd.setCursor(0, 0);
  lcd.print(F("Soil: "));
  lcd.print(soilMoisturePercent);
  lcd.print("%    ");

  lcd.setCursor(0, 1);
  // Check conditions to turn on the pump
  if (soilMoisturePercent < soilMoistureThreshold && waterLevel < minWaterLevel) {
    digitalWrite(pumpPin, HIGH);
    lcd.print("Pump ON                  ");
  } else {
    digitalWrite(pumpPin, LOW);
    if (waterLevel >= minWaterLevel) {
      lcd.print("Low water level        ");
    } else {
      lcd.print("Soil moisture ok       ");
    }
  }
}

long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;  // Convert to cm
  return distance;
}
