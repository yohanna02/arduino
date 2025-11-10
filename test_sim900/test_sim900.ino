#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// SIM900 RX/TX
SoftwareSerial sim900(6, 5);  // RX, TX

// SIM900 PWRKEY pin
const int PWRKEY_PIN = 7;

// Fixed phone number to send SMS to
const char FIXED_NUMBER[] = "+2349022107944";  // change to your number

// I2C LCD at address 0x27 (change if needed)
LiquidCrystal_I2C lcd(0x27, 16, 2);

String incomingData = "";
String smsContent = "";

void setup() {
  pinMode(PWRKEY_PIN, OUTPUT);
  digitalWrite(PWRKEY_PIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SIM900 Test");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  sim900.begin(9600);

  powerOnSIM900();
  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Init SMS...");
  initSMS();

  lcd.setCursor(0, 1);
  lcd.print("Ready for SMS");
}

void loop() {
  if (sim900.available()) {
    char c = sim900.read();
    incomingData += c;

    if (c == '\n') {
      processLine(incomingData);
      incomingData = "";
    }
  }
}

void powerOnSIM900() {
  digitalWrite(PWRKEY_PIN, LOW);
  delay(2000);
  digitalWrite(PWRKEY_PIN, HIGH);
  delay(2000);
}

void initSMS() {
  sim900.println("AT");
  delay(500);
  sim900.println("AT+CMGF=1");
  delay(500);  // text mode
  sim900.println("AT+CNMI=1,2,0,0,0");
  delay(500);  // push new SMS
}

void processLine(String line) {
  line.trim();
  if (line.startsWith("+CMT:")) {
    smsContent = "";  // header line
  } else if (smsContent == "") {
    // actual message text line
    smsContent = line;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SMS Received:");
    lcd.setCursor(0, 1);
    lcd.print(smsContent.substring(0, 16));  // show first 16 chars

    // check your command
    if (smsContent.indexOf("STATUS") != -1) {
      sendSMS(FIXED_NUMBER, "Device is ON");
    }

    smsContent = "";
  }
}

void sendSMS(const char *number, const char *text) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sending SMS...");

  sim900.print("AT+CMGS=\"");
  sim900.print(number);
  sim900.println("\"");
  delay(500);

  sim900.print(text);
  sim900.write(26);  // Ctrl+Z to send
  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SMS Sent!");
}
