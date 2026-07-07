#define BLYNK_TEMPLATE_ID "TMPL2QjeAH14j"
#define BLYNK_TEMPLATE_NAME "Meter"
#define BLYNK_AUTH_TOKEN "eFGIEoJ697SXVwEj3ZafyjUAMkQhn4J_"

#include <Keypad.h>
#include <HardwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include "ACS712.h"
#include <WiFi.h>

#include <WiFiClient.h>

#include <BlynkSimpleEsp32.h>

// ------------------- PIN DEFINITIONS -------------------
#define SENSOR_IN 34
#define SENSOR_OUT 35
// #define PIR_PIN 4
#define RELAY_PIN 18

char ssid[] = "@OIC";
char pass[] = "oichub@@1930";


ACS712 ACS_1(SENSOR_IN, 3.3, 4095, 185.0);
ACS712 ACS_2(SENSOR_OUT, 3.3, 4095, 185.0);

LiquidCrystal_I2C lcd(0x27, 16, 2);

HardwareSerial sim800(2);  // UART2: RX=16, TX=17

// ------------------- KEYPAD SETUP -------------------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 32, 33, 25, 26 };
byte colPins[COLS] = { 27, 14, 12, 13 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ------------------- SYSTEM VARIABLES -------------------
String unlockCode = "";
String inputCode = "";

bool locked = false;

float minimalLoad = 0.26;
float bypassThreshold = 0.40;

int triggerCount = 0;

bool showDebug = false;

// ------------------- SETUP -------------------
void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  lcd.init();
  lcd.backlight();
  sim800.begin(9600, SERIAL_8N1, 16, 17);

  lcd.print(F("Connecting..."));
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  lcd.clear();
  lcd.print(F("Connected"));
  delay(2000);
  lcd.clear();


  lcd.print(F("Calibrating..."));
  ACS_1.autoMidPoint();
  ACS_2.autoMidPoint();
  delay(3000);
  lcd.clear();
  digitalWrite(RELAY_PIN, HIGH);

  lcd.print(F("Development of"));
  lcd.setCursor(0, 1);
  lcd.print(F("an Iot-based"));
  delay(3000);
  lcd.clear();

  lcd.print(F("anti-theft"));
  lcd.setCursor(0, 1);
  lcd.print(F("device in"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Electricity"));
  lcd.setCursor(0, 1);
  lcd.print(F("Meters"));
  delay(3000);
  lcd.clear();
}

// ------------------- MAIN LOOP -------------------
void loop() {
  Blynk.run();
  // -------- KEYPAD INPUT --------
  char key = keypad.getKey();
  if (locked) {
    if (key == '#') {
      lcd.clear();
      lcd.print(F("Enter Pin"));
      delay(1000);

      while (locked) {
        char k = keypad.getKey();
        if (k == '#') {
          if (inputCode == unlockCode) {
            locked = false;
            lcd.clear();
            lcd.print(F("Unlock"));
            lcd.setCursor(0, 1);
            lcd.print(F("successful"));
            delay(2000);
            lcd.clear();
            digitalWrite(RELAY_PIN, HIGH);
          } else {
            lcd.clear();
            lcd.print(F("Wrong code"));
            delay(2000);
            lcd.clear();
            lcd.print(F("Enter Pin"));
          }
          inputCode = "";
        } else if (k >= '0' && k <= '9') {
          inputCode += k;
          lcd.setCursor(0, 1);
          lcd.print(inputCode);
        }
      }
    }
  }

  if (locked) {
    digitalWrite(RELAY_PIN, LOW);
    lcd.setCursor(0, 0);
    lcd.print(F("Locked!!!     "));
    lcd.setCursor(0, 1);
    lcd.print(F("# to unlock   "));
    return;
  }

  // -------- CURRENT MEASUREMENT --------
  float currentIn = readCurrentWithCheck(ACS_1);
  float currentOut = readCurrentWithCheck(ACS_2);
  Blynk.virtualWrite(V0, currentIn);
  Blynk.virtualWrite(V1, currentOut);

  if (currentIn > bypassThreshold && !locked) {

    triggerCount++;
    if (triggerCount == 1) return;

    lcd.clear();
    lcd.print(F("Meter Bypass"));
    lcd.setCursor(0, 1);
    lcd.print(F("alert!!!"));
    unlockCode = generateCode();
    sendSMS("Meter Bypass alert! Unlock code: " + unlockCode);
    Blynk.logEvent("bypass", String("Meter Bypass alert!!! Unlock code: ") + unlockCode);
    Blynk.run();
    delay(2000);
    lcd.clear();
    locked = true;
    return;
  }

  if (!showDebug) {
    lcd.setCursor(0, 0);
    lcd.print(F("System OK         "));
    lcd.setCursor(0, 1);
    lcd.print(F("                  "));
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print(F("In  "));
    lcd.print(currentIn);
    lcd.print(F("   "));
    lcd.setCursor(0, 1);
    lcd.print(F("   "));
    lcd.print(F("Out  "));
    lcd.print(currentOut);
    lcd.print(F("   "));
  }
}

// ------------------- SEND SMS -------------------
void sendSMS(String msg) {
  sim800.println("AT+CMGF=1");
  delay(500);
  sim800.println("AT+CMGS=\"+2349022107944\"");
  // sim800.println("AT+CMGS=\"+2348033667937\"");
  delay(500);
  sim800.print(msg);
  delay(500);
  sim800.write(26);
}

// ------------------- READ CURRENT -------------------
float readCurrentWithCheck(ACS712 sensor) {
  const int NUM_READINGS = 20;

  long sum = 0.0;
  for (int i = 0; i < NUM_READINGS; i++) {
    sum += sensor.mA_AC();
  }

  float mA = sum / NUM_READINGS;

  if (mA <= 90) {
    return 0.0;
  }

  return mA / 1000.0;
}

// ------------------- GENERATE 4-DIGIT CODE -------------------
String generateCode() {
  String code = "";
  for (int i = 0; i < 4; i++) {
    code += String(random(0, 10));
  }
  return code;
}
