// Solar Powered Water Pollution Detector
// Components:
// ESP32
// Turbidity Sensor
// 16x2 LCD (Without I2C)
// Buzzer

#include <LiquidCrystal.h>

// LCD PINSqq
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(14, 27, 26, 25, 33, 32);

// Turbidity Sensor Pin
const int turbidityPin = 35;

// Buzzer Pin
const int buzzerPin = 18;
const int ledPin = 19;

// Variables
int sensorValue = 0;
float voltage = 0.0;
float turbidity = 0.0;

void setup() {

  // Initialize LCD
  lcd.begin(16, 2);

  // Initialize Serial Monitor
  // Serial.begin(115200);

  // Set buzzer as output
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // Welcome Message
  lcd.setCursor(0, 0);
  lcd.print("WATER QUALITY");

  lcd.setCursor(0, 1);
  lcd.print("MONITOR SYSTEM");

  delay(3000);
  lcd.clear();
}

void loop() {

  // Read turbidity sensor
  sensorValue = analogRead(turbidityPin);

  // Display Water Condition
  lcd.setCursor(0, 0);

  // CLEAN WATER
  if (sensorValue > 1700) {

    lcd.print("Water: CLEAN ");
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);

  } else {

    lcd.print("Water: DIRTY ");

    // Activate buzzer
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);
    delay(500);
  }

  lcd.setCursor(0, 1);
  lcd.print(sensorValue);
  lcd.print(F("     "));
}
