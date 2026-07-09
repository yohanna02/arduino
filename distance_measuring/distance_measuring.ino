#include <LiquidCrystal_I2C.h>

// LCD Pins
LiquidCrystal_I2C lcd(0x27, 16, 2);
// LiquidCrystal lcd(12, A0, A1, A2, A4, A3);

// Ultrasonic Pins
const int trigPin = 7;
const int echoPin = 8;

long duration;
float distance;

#define BUZZER 5

void setup()
{
  pinMode(BUZZER, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lcd.begin(16, 2);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Design and");
  lcd.setCursor(0,1);
  lcd.print("construction");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("of distance");
  lcd.setCursor(0,1);
  lcd.print("measuring system");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("By Computer");
  lcd.setCursor(0,1);
  lcd.print("Engineering");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("students");
  delay(2000);

  lcd.clear();
}

void loop()
{
  // Clear Trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send 10us pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read Echo
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance in cm
  distance = duration * 0.0343 / 2.0;

  lcd.setCursor(0,0);
  lcd.print("Distance:");

  lcd.setCursor(0,1);
  lcd.print(distance,1);
  lcd.print(" cm      ");

  if (distance <= 10) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  delay(200);
}