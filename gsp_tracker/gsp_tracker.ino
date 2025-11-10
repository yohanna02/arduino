#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <TinyGPS++.h>

// #define PHONE_NUMBER "+2349022107944"
#define PHONE_NUMBER "+2348066734848"

// GPS connected via SoftwareSerial
SoftwareSerial gpsSerial(2, 3);  // GPS RX, TX
TinyGPSPlus gps;

// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

// Buffers
bool locationReady = false;
float latitude, longitude;

#define BUZZER 13
#define BTN 6

void setup() {
  pinMode(BUZZER, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);
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
  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("construction"));
  delay(3000);
  lcd.clear();

  lcd.print(F("of a GPS"));
  lcd.setCursor(0, 1);
  lcd.print(F("tracking device"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Ibrahim Lawan"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/149197"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Mr.Mushinwa Ikechukwu.E"));
  delay(3000);
  lcd.clear();

  lcd.print("Getting Location");
}

void loop() {
  // --- GPS Reading ---
  if (gpsSerial.available()) {
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
    String smsData = Serial.readString();
    smsData.toUpperCase();  // normalize


    if (smsData.indexOf("TRACK") != -1) {
      digitalWrite(BUZZER, HIGH);
      lcd.clear();
      lcd.print(F("Sending"));
      lcd.setCursor(0, 1);
      lcd.print(F("location"));
      delay(3000);
      sendLocationSMS();
      digitalWrite(BUZZER, LOW);
      lcd.clear();
      while (Serial.available()) Serial.read();
    }
  }

  if (!digitalRead(BTN)) {
    digitalWrite(BUZZER, HIGH);
    lcd.clear();
    lcd.print(F("Sending"));
    lcd.setCursor(0, 1);
    lcd.print(F("location"));
    delay(3000);
    sendLocationSMS();
    digitalWrite(BUZZER, LOW);
    lcd.clear();
  }
}

// --- Send Location via SMS ---
void sendLocationSMS() {
  // if (locationReady) {
  String message = "Location: https://maps.google.com/?q=";
  message += String(latitude, 6);
  message += ",";
  message += String(longitude, 6);

  sendCommand("AT+CMGF=1", 1000);
  sendCommand("AT+CMGS=\"" PHONE_NUMBER "\"", 1000);
  Serial.print(message);
  Serial.write(26);  // CTRL+Z to send

  lcd.clear();
  lcd.print("SMS Sent!");
  delay(2000);
  // } else {
  //   lcd.clear();
  //   lcd.print("No Location yet");
  //   delay(2000);
  // }
}

// --- Helper to send AT commands ---
void sendCommand(String cmd, int wait) {
  Serial.println(cmd);
  delay(wait);
  while (Serial.available()) {
    Serial.read();  // Clear response
  }
}
