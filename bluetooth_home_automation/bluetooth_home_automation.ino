#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Appliance pins
#define SOCKET 2
#define BULB   3
#define FAN    4
#define TV     5

char command; // Store incoming Bluetooth command

void setup() {
  // Set appliance pins as output
  pinMode(SOCKET, OUTPUT);
  pinMode(BULB, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(TV, OUTPUT);

  // All OFF initially
  digitalWrite(SOCKET, LOW);
  digitalWrite(BULB, LOW);
  digitalWrite(FAN, LOW);
  digitalWrite(TV, LOW);

  Serial.begin(9600);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Home Automation");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");
  delay(2000);
}

void loop() {
  if (Serial.available()) {
    command = Serial.read();

    switch (command) {
      case 'A': // Toggle Socket
        digitalWrite(SOCKET, !digitalRead(SOCKET));
        lcd.clear();
        lcd.print("Socket: ");
        lcd.print(digitalRead(SOCKET) ? "ON" : "OFF");
        break;

      case 'B': // Toggle Bulb
        digitalWrite(BULB, !digitalRead(BULB));
        lcd.clear();
        lcd.print("Bulb: ");
        lcd.print(digitalRead(BULB) ? "ON" : "OFF");
        break;

      case 'C': // Toggle Fan
        digitalWrite(FAN, !digitalRead(FAN));
        lcd.clear();
        lcd.print("Fan: ");
        lcd.print(digitalRead(FAN) ? "ON" : "OFF");
        break;

      case 'D': // Toggle TV
        digitalWrite(TV, !digitalRead(TV));
        lcd.clear();
        lcd.print("TV: ");
        lcd.print(digitalRead(TV) ? "ON" : "OFF");
        break;

      case 'E': // All ON
        digitalWrite(SOCKET, HIGH);
        digitalWrite(BULB, HIGH);
        digitalWrite(FAN, HIGH);
        digitalWrite(TV, HIGH);
        lcd.clear();
        lcd.print("All Devices ON");
        break;

      case 'F': // All OFF
        digitalWrite(SOCKET, LOW);
        digitalWrite(BULB, LOW);
        digitalWrite(FAN, LOW);
        digitalWrite(TV, LOW);
        lcd.clear();
        lcd.print("All Devices OFF");
        break;
    }
  }
}
