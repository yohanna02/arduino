#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <ESPComm.h>
#include <HX711.h>

SoftwareSerial _serial(4, 3);
LiquidCrystal_I2C lcd(0x27, 16, 2);
ESPComm esp(Serial);

#define GSM_SERIAL _serial

#define PWRKEY_PIN A2
#define NUMBER "+2347064391008"

#define LOADCELL_DOUT_PIN 2
#define LOADCELL_SCK_PIN 13
#define GAS_PIN A0
#define BUZZER_PIN A1

#define BTN_KG3 6
#define BTN_KG6 5

// HX711
HX711 scale;
int reading;
int lastReading;

#define CALIBRATION_FACTOR 86.413695652

// Cylinder selection
float cylinderCapacity = 0;  // grams
bool cylinderSelected = false;

// Gas sensor threshold (adjust experimentally)
#define GAS_THRESHOLD 450

String incomingData = "";
String smsContent = "";
bool sent = false;

unsigned long prevMillis = 0;
unsigned long displayMillis = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(PWRKEY_PIN, OUTPUT);
  digitalWrite(PWRKEY_PIN, LOW);
  esp.begin(9600);
  powerOnSIM900();
  GSM_SERIAL.begin(9600);
  initSMS();

  pinMode(BTN_KG3, INPUT_PULLUP);
  pinMode(BTN_KG6, INPUT_PULLUP);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GAS_PIN, INPUT);


  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR);
  scale.tare();

  lcd.setCursor(0, 0);
  lcd.print("Select Cylinder:");
  lcd.setCursor(0, 1);
  lcd.print("3=3kg, 6=6kg");
}

void loop() {
  // If cylinder not selected yet
  if (!cylinderSelected) {
    if (!digitalRead(BTN_KG3)) {
      cylinderCapacity = 3000;  // grams
      cylinderSelected = true;
      lcd.clear();
      lcd.print("3kg Cylinder Sel");
      delay(1500);
      lcd.clear();
    } else if (!digitalRead(BTN_KG6)) {
      cylinderCapacity = 6000;  // grams
      cylinderSelected = true;
      lcd.clear();
      lcd.print("6kg Cylinder Sel");
      delay(1500);
      lcd.clear();
    }
    return;
  }

  // --- Weight Monitoring ---
  if (scale.wait_ready_timeout(200)) {
    reading = round(scale.get_units());

    unsigned long currentMillis = millis();

    if (reading != lastReading && currentMillis - displayMillis > 4000) {
      float percent = (reading / cylinderCapacity) * 100.0;
      if (percent < 0) percent = 0;  // avoid negative

      lcd.setCursor(0, 0);
      lcd.print("Weight: ");
      lcd.print(reading / 1000);
      lcd.print("kg           ");

      lcd.setCursor(0, 1);
      lcd.print("Level: ");
      lcd.print(percent, 1);
      lcd.print("%           ");

      if (currentMillis - prevMillis > 20000) {
        esp.send("PERCENT", percent);
        prevMillis = millis();
      }

      lastReading = reading;
      displayMillis = millis();
    }
  }

  // --- Gas Leakage Detection ---
  int gasValue = analogRead(GAS_PIN);
  if (gasValue > GAS_THRESHOLD) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!! GAS LEAKAGE !!");
    lcd.setCursor(0, 1);
    lcd.print("Value:");
    lcd.print(gasValue);

    // Trigger buzzer
    digitalWrite(BUZZER_PIN, HIGH);

    if (!sent) {
      sent = true;
      sendSMS(NUMBER, "GAS LEAKAGE DETECTED!!!");
      esp.send("GAS_DETECTED", 1);
    }

    delay(2000);  // Show warning for 2s
    lcd.clear();
  } else {
    if (sent) {
      sent = false;
      esp.send("GAS_DETECTED", 0);
    }
  }

  if (GSM_SERIAL.available()) {
    char c = GSM_SERIAL.read();
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
  GSM_SERIAL.println("AT");
  delay(500);
  GSM_SERIAL.println("AT+CMGF=1");
  delay(500);  // text mode
  GSM_SERIAL.println("AT+CNMI=1,2,0,0,0");
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
      float percent = (reading / cylinderCapacity) * 100.0;
      String message = "Cylinder at " + String(percent) + "%";
      sendSMS(NUMBER, message.c_str());
    }

    lcd.clear();
    smsContent = "";
  }
}

void sendSMS(const char *number, const char *text) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sending SMS...");

  GSM_SERIAL.print("AT+CMGS=\"");
  GSM_SERIAL.print(number);
  GSM_SERIAL.println("\"");
  delay(500);

  GSM_SERIAL.print(text);
  GSM_SERIAL.write(26);  // Ctrl+Z to send
  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SMS Sent!");
}
