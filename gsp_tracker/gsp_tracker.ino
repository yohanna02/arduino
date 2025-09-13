#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <TinyGPS++.h>


#define PHONE_NUMBER "+234XXXXXXXXXX"

// GPS connected via SoftwareSerial
SoftwareSerial gpsSerial(4, 3);  // GPS RX, TX
TinyGPSPlus gps;

// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Buffers
String smsBuffer = "";
bool locationReady = false;
float latitude, longitude;

void setup() {
  lcd.begin(16, 2);
  lcd.print("Initializing...");

  // Serial for GSM (SIM800L)
  Serial.begin(9600);     // GSM on hardware Serial
  gpsSerial.begin(9600);  // GPS

  delay(1000);

  // Initialize GSM
  sendCommand("AT", 1000);
  sendCommand("AT+CMGF=1", 1000);          // SMS text mode
  sendCommand("AT+CNMI=1,2,0,0,0", 1000);  // Forward SMS to serial immediately

  lcd.clear();
  lcd.print("Getting Location");
}

void loop() {
  // --- GPS Reading ---
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
    if (gps.location.isUpdated()) {
      latitude = gps.location.lat();
      longitude = gps.location.lng();
      locationReady = true;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Lat:");
      lcd.print(latitude, 2);

      lcd.setCursor(0, 1);
      lcd.print("Lng:");
      lcd.print(longitude, 2);
    }
  }

  // --- GSM SMS Listening ---
  while (Serial.available()) {
    char c = Serial.read();
    smsBuffer += c;

    if (c == '\n') {
      smsBuffer.trim();
      if (smsBuffer.indexOf("track") != -1) {
        sendLocationSMS();
      }
      smsBuffer = "";
    }
  }
}

// --- Send Location via SMS ---
void sendLocationSMS() {
  if (locationReady) {
    String message = "Location: https://maps.google.com/?q=";
    message += String(latitude, 6);
    message += ",";
    message += String(longitude, 6);

    sendCommand("AT+CMGS=\"" PHONE_NUMBER "\"", 1000);
    Serial.print(message);
    Serial.write(26);  // CTRL+Z to send

    lcd.clear();
    lcd.print("SMS Sent!");
    delay(2000);
  } else {
    lcd.clear();
    lcd.print("No Location yet");
    delay(2000);
  }
}

// --- Helper to send AT commands ---
void sendCommand(String cmd, int wait) {
  Serial.println(cmd);
  delay(wait);
  while (Serial.available()) {
    Serial.read();  // Clear response
  }
}
