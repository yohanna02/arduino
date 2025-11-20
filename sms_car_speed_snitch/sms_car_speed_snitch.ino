#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

SoftwareSerial gsm(8, 9);

String plateNumbers[4][2] = {
  { "GRE-165RC", "+2348068111733" },
  { "KJA-193AA", "+2348068111733" },
  { "TRR-791YX", "+2348068111733" },
  { "BBU-333XA", "+2348068111733" }
  // { "BBU-333XA", "+2347032153315" }
};

String lastSent = "";

#define GSM_SERIAL gsm

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  GSM_SERIAL.begin(9600);
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
    // lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.print(Serial.readString());
    // lcd.print(F("    "));

    for (int i = 0; i < 4; i++) {
      if (plateNumbers[i][0] == plateNumber) {
        lcd.clear();
        lcd.print(plateNumber);
        lcd.setCursor(0, 1);
        lcd.print(speed);
        // if (lastSent != plateNumber) {
          String data = "The car with " + plateNumber + " has violated traffic. Speed: " + String(speed);
          sendSMS(data, plateNumbers[i][1]);
        // }
        delay(2000);
        lcd.clear();
        lastSent = plateNumber;
      }
    }
  }
}

void sendSMS(String message, String number) {
  GSM_SERIAL.println("AT+CMGF=1");  // Set SMS mode to text
  delay(500);
  GSM_SERIAL.println("AT+CMGS=\"" + number + "\"");  // Replace with your phone number
  delay(500);
  GSM_SERIAL.print(message);
  GSM_SERIAL.write(26);  // Ctrl+Z to send SMS
  delay(1000);
}