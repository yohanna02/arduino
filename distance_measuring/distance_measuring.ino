#include <LiquidCrystal.h>

// LCD Pins
LiquidCrystal lcd(12, A0, A1, A2, A4, A3);

// Ultrasonic Pins
const int trigPin = 3;
const int echoPin = 2;

long duration;
float distance;

void setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lcd.begin(16, 2);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Digital distance");
  lcd.setCursor(0,1);
  lcd.print("measuring device");

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

  delay(200);
}