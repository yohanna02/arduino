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
  lcd.print(F("Construction of"));
  lcd.setCursor(0, 1);
  lcd.print(F("smart irrigation"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("system using"));
  lcd.setCursor(0, 1);
  lcd.print(F("moisture senor"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Sohbin Nanman Tyem"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/150211"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Usman Abdulhamid"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/150688"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Sixtus Kighir"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/152263"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Jibrin Habibu"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151229"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Bitrus Deborah"));
  lcd.setCursor(0, 1);
  lcd.print(F("21/144565"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Mal.Adebayo Abdullahi"));
  delay(3000);
  lcd.clear();

  // lcd.print(F("Design and"));
  // lcd.setCursor(0, 1);
  // lcd.print(F("construction"));
  // delay(3000);

  // lcd.clear();
  // lcd.print(F("of a digital"));
  // lcd.setCursor(0, 1);
  // lcd.print(F("soil moisture sensor"));
  // delay(3000);

  // lcd.clear();
  // lcd.print(F("Anaja Faith Ejuile"));
  // lcd.setCursor(0, 1);
  // lcd.print(F("17/48932U/2"));
  // delay(3000);

  // lcd.clear();
  // lcd.print(F("Supervised by"));
  // lcd.setCursor(0, 1);
  // lcd.print(F("Prof. H.Kabri"));
  // delay(3000);
  // lcd.clear();
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
