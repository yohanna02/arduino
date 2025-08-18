#include <LiquidCrystal.h>

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

// Pins
const int signalPin = A0;    // Analog signal input from LM358
const int ledPin = 6;        // Yellow LED
const int buzzerPin = 5;     // Buzzer

// Threshold for GSM detection (to be adjusted after testing)
const int detectionThreshold = 320;  // Approx ~1.5V after op-amp

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  lcd.begin(16, 2); 
  lcd.setCursor(0,0);
  lcd.print("GSM Detector");
  lcd.setCursor(1,0);
  lcd.print("Ready");
  delay(1000);
  lcd.clear();
}

void loop() {
  int analogVal = analogRead(signalPin);          // value (0–1023)
  float percentage = (analogVal / 1023.0) * 100;   // Convert to %

  lcd.setCursor(0, 0);
  lcd.print("Signal: ");
  lcd.print(percentage, 1);  
  lcd.print("%   ");         

  if (analogVal > detectionThreshold) {
    // GSM signal detected
    lcd.setCursor(0, 1);
    lcd.print("GSM Signal Found ");
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(300); // Short buzzer alert
    digitalWrite(buzzerPin, LOW);
  } else {
    // No signal
    lcd.setCursor(0, 1);
    lcd.print("Searching Signal");
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }

  delay(300); 
}
