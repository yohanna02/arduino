#include <LiquidCrystal.h>

LiquidCrystal lcd(10, 11, 4, 5, 6, 7);

String plateNumbers[4][2] = {
  { "KJA-825CQ", "+2349022107944" },
  { "AY174-LSR", "+2348068111733" },
  { "MNA-914JL", "+2348068111733" },
  { "BBU-333XA", "+2348068111733" }
  // { "BBU-333XA", "+2347032153315" }
};

String lastSent = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  lcd.begin(16, 2);
  lcd.print(F("Initializing"));
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0, 0);
  lcd.print(F("Monitoring..."));
  if (Serial.available()) {
    String plateNumber = Serial.readStringUntil(' ');
    float speed = Serial.readString().toFloat();

    if (lastSent == plateNumber) {
      return;
    }

    if (plateNumbers[0][0] == plateNumber) {
      lcd.clear();
      lcd.print(plateNumber);
      lcd.setCursor(0, 1);
      lcd.print(speed);

      digitalWrite(2, HIGH);
      delay(1000);
      digitalWrite(2, LOW);
      lcd.clear();
      Serial.readString();
    }
    else if (plateNumbers[1][0] == plateNumber) {
      lcd.clear();
      lcd.print(plateNumber);
      lcd.setCursor(0, 1);
      lcd.print(speed);

      digitalWrite(3, HIGH);
      delay(1000);
      digitalWrite(3, LOW);
      lcd.clear();
      Serial.readString();
    }
    else if (plateNumbers[2][0] == plateNumber) {
      lcd.clear();
      lcd.print(plateNumber);
      lcd.setCursor(0, 1);
      lcd.print(speed);

      digitalWrite(8, HIGH);
      delay(1000);
      digitalWrite(8, LOW);
      lcd.clear();
      Serial.readString();
    }
    else if (plateNumbers[3][0] == plateNumber) {
      lcd.clear();
      lcd.print(plateNumber);
      lcd.setCursor(0, 1);
      lcd.print(speed);

      digitalWrite(9, HIGH);
      delay(1000);
      digitalWrite(9, LOW);
      lcd.clear();
      Serial.readString();
    }

    lastSent = plateNumber;
  }
}

// void sendSMS(String message, String number) {
//   GSM_SERIAL.println("AT+CMGF=1");  // Set SMS mode to text
//   delay(500);
//   GSM_SERIAL.println("AT+CMGS=\"" + number + "\"");  // Replace with your phone number
//   delay(500);
//   GSM_SERIAL.print(message);
//   GSM_SERIAL.write(26);  // Ctrl+Z to send SMS
//   delay(1000);
// }