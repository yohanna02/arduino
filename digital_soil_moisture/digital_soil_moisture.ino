#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SOIL_MOISTURE_PIN A0
#define PUMP_PIN 9

#define WATER_LEVEL_ON_PERCENT 50
#define WATER_LEVEL_OFF_PERCENT 70

// Ultrasonic sensor pins
const int trigPin = 7;
const int echoPin = 8;

const float tankDepth = 14.0;                  // cm (set to your tank height)
const float lowLevelThresholdPercent = 30.0;   // below this, pump ON
const float highLevelThresholdPercent = 70.0;  // above this, pump OFF

void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lcd.init();
  lcd.backlight();

  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("implementation"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("of a plant health"));
  lcd.setCursor(0, 1);
  lcd.print(F("monitoring system"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Stephen Matthew ogbe"));
  lcd.setCursor(0, 1);
  lcd.print(F("BPU/EEE/H/EEP/23/10027"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Odoh Gabriel"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr.justin kpelai.A"));
  delay(3000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:

  // float distance = readUltrasonic();

  // Water level = tankDepth - distanceFromSensor
  // float waterLevel = tankDepth - distance;
  // if (waterLevel < 0) waterLevel = 0;
  // if (waterLevel > tankDepth) waterLevel = tankDepth;

  // float waterPercent = (waterLevel / tankDepth) * 100.0;

  int soilRawValue = analogRead(SOIL_MOISTURE_PIN);

  int soilLevel = map(soilRawValue, 200, 500, 100, 0);

  soilLevel = constrain(soilLevel, 0, 100);

  lcd.setCursor(0, 0);
  lcd.print(F("W Level:"));
  lcd.print(soilLevel);
  lcd.print(F("%   "));

  lcd.setCursor(0, 1);
  // if (waterPercent > 30) {
  if (soilLevel <= WATER_LEVEL_ON_PERCENT) {
    digitalWrite(PUMP_PIN, HIGH);
    lcd.print(F("Pump ON    "));
  } else if (soilLevel >= WATER_LEVEL_OFF_PERCENT) {
    digitalWrite(PUMP_PIN, LOW);
    lcd.print(F("Pump OFF   "));
  } else {
    lcd.print(F("           "));
  }
  // } else {
  //   digitalWrite(PUMP_PIN, LOW);
  //   lcd.print(F("Tank Empty   "));
  // }
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
