#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD at 0x27 address (common); adjust if needed
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ultrasonic sensor pins
const int trigPin = 8;
const int echoPin = 7;

// Relay and buzzer pins
const int relayPin = 10;
const int buzzerPin = 4;

// Tank parameters
const float tankDepth = 14.0;                  // cm (set to your tank height)
const float lowLevelThresholdPercent = 30.0;   // below this, pump ON
const float highLevelThresholdPercent = 70.0;  // above this, pump OFF

// Track pump state
bool pumpOn = false;

const int redLed = 2;
const int greenLed = 6;
const int yellowLed = 1;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);

  digitalWrite(relayPin, LOW);   // Pump off initially
  digitalWrite(buzzerPin, LOW);  // Buzzer off initially

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Water Level Sys");
  delay(2000);
  lcd.clear();

  lcd.print(F("Construction of"));
  lcd.setCursor(0, 1);
  lcd.print(F("automated water"));
  delay(3000);
  lcd.clear();

  lcd.print(F("level indicator"));
  lcd.setCursor(0, 1);
  lcd.print(F("and pump"));
  delay(3000);
  lcd.clear();

  lcd.print(F("control"));
  lcd.setCursor(0, 1);
  lcd.print(F("system pump"));
  delay(3000);
  lcd.clear();

  lcd.print(F("HARUNA MUSA"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/150834"));
  delay(3000);
  lcd.clear();

  lcd.print(F("MUSA IBRAHIM"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151065"));
  delay(3000);
  lcd.clear();

  lcd.print(F("SILAS OMEGA ZURIGA"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151222"));
  delay(3000);
  lcd.clear();

  lcd.print(F("BALA ABDULRAHMAN ISHIYAKU"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151223"));
  delay(3000);
  lcd.clear();

  lcd.print(F("SAIDU ABUBAKAR HASSAN"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151537"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Mal Aliyu Abdulrahman"));
  delay(3000);
  lcd.clear();
}

void loop() {
  float distance = readUltrasonic();

  // Water level = tankDepth - distanceFromSensor
  float waterLevel = tankDepth - distance;
  if (waterLevel < 0) waterLevel = 0;
  if (waterLevel > tankDepth) waterLevel = tankDepth;

  lcd.setCursor(0, 0);
  lcd.print(waterLevel);
  lcd.print(F("   "));

  // Calculate percentage
  float waterPercent = (waterLevel / tankDepth) * 100.0;

  // Show % on LCD
  lcd.setCursor(0, 0);
  lcd.print("Level: ");
  lcd.print(waterPercent, 1);
  lcd.print("%    ");  // spaces clear old chars

  if (waterPercent <= lowLevelThresholdPercent) {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
  } else if (waterPercent > lowLevelThresholdPercent && waterPercent <= 70) {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, HIGH);
  } else if (waterPercent > 70) {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    digitalWrite(yellowLed, LOW);
  }

  // Pump control with hysteresis in percentage
  if (!pumpOn && waterPercent < lowLevelThresholdPercent) {
    // Turn pump ON
    pumpOn = true;
  } else if (pumpOn && waterPercent > highLevelThresholdPercent) {
    // Turn pump OFF
    pumpOn = false;
  }

  // Apply pump state
  if (pumpOn) {
    digitalWrite(relayPin, HIGH);  // Pump ON
    tone(buzzerPin, 2000);         // 2kHz tone while pump is ON
  } else {
    digitalWrite(relayPin, LOW);  // Pump OFF
    noTone(buzzerPin);            // Buzzer OFF
  }

  // LCD pump status
  lcd.setCursor(0, 1);
  if (pumpOn) {
    lcd.print("Pump: ON        ");
  } else {
    lcd.print("Pump: OFF       ");
  }

  delay(1000);  // update every second
}

float readUltrasonic() {
  // Send pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo time
  long duration = pulseIn(echoPin, HIGH);

  // Calculate distance in cm
  float distance = duration * 0.0343 / 2;

  return distance;
}
