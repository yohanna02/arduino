#define BLYNK_TEMPLATE_ID "TMPL2ieJlHP-7"
#define BLYNK_TEMPLATE_NAME "Energy Meter"
#define BLYNK_AUTH_TOKEN "6CGqczWSWpn-_w6vrg1hQf13q_VjxUTv"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include "ACS712.h"
#include <ZMPT101B.h>
#include <HardwareSerial.h>

HardwareSerial mySerial(2);  // Use UART1

#define sim800 mySerial

char ssid[] = "OIC_";
char pass[] = "oichub@@1940";

#define VOLTAGE_SENSOR_PIN 35
#define CURRENT_SENSOR_PIN 34

#define RELAY_PIN 23

LiquidCrystal_I2C lcd(0x27, 16, 2);

ACS712 current_sensor(ACS712_05B, CURRENT_SENSOR_PIN);
ZMPT101B voltageSensor(VOLTAGE_SENSOR_PIN, 50.0);

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 32, 33, 25, 26 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 27, 14, 12, 13 };  //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

float energyLimit = 0.0;
float realPower = 0.0;

unsigned long prevMillis = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(RELAY_PIN, OUTPUT);
  sim800.begin(9600, SERIAL_8N1, 16, 17);  // RX=25, TX=26

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  lcd.print(F("Calibrating"));
  lcd.setCursor(0, 1);
  lcd.print(F("System"));

  current_sensor.calibrate();

  // --- Initialize SIM800 ---
  sim800.println("AT");
  delay(1000);
  sim800.println("AT+CMGF=1");  // Set SMS mode
  delay(1000);
  sim800.println("AT+CNMI=1,2,0,0,0");  // Auto-display incoming SMS
  delay(1000);

  lcd.clear();

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("Construction"));
  delay(3000);
  lcd.clear();

  lcd.print(F("of an IoT based"));
  lcd.setCursor(0, 1);
  lcd.print(F("energy meter"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Joshua Mathias"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151745"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Isreal Haingowa"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151774"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr Tech.Dahiru Zalani"));
  delay(3000);
  lcd.clear();

  digitalWrite(RELAY_PIN, HIGH);
}

void loop() {
  Blynk.run();
  // Your code here

  char key = keypad.getKey();

  if (key == 'D') {
    String value = "";
    lcd.clear();
    lcd.print(F("Set Energy"));
    while (true) {
      char key = keypad.getKey();

      if (key >= '0' && key <= '9') {
        value += key;
      }

      else if (key == '*') {
        value = "";
        lcd.clear();
        lcd.print(F("Set Energy"));
      }

      else if (key == '#') {
        energyLimit = value.toInt();
        Blynk.virtualWrite(V1, energyLimit);
        value = "";
        break;
      }

      lcd.setCursor(0, 1);
      lcd.print(value);
    }
  }

  float Vrms = 220;
  float Irms = readCurrentWithCheck(current_sensor);
  realPower = Vrms * Irms;

  lcd.setCursor(0, 0);
  lcd.print(F("Load: "));
  lcd.print(realPower);
  lcd.print(F("W     "));

  lcd.setCursor(0, 1);
  if (energyLimit == 0) {
    lcd.print(F("No limit set      "));
  } else {
    lcd.print(F("Limit: "));
    lcd.print(energyLimit);
    lcd.print(F("W                 "));
  }

  if (millis() - prevMillis > 3000) {
    Blynk.virtualWrite(V0, realPower);

    prevMillis = millis();
  }

  if (realPower > energyLimit && energyLimit != 0) {
    digitalWrite(RELAY_PIN, LOW);
  }

  // --- 🔹 Check for incoming SMS ---
  checkForIncomingSMS();
}

BLYNK_WRITE(V1) {
  // Add your code here to act upon Message change
  energyLimit = param.asInt();
  lcd.clear();
  lcd.print(F("Energy Limit"));
  lcd.setCursor(0, 1);
  lcd.print(F("set"));
  delay(3000);
  lcd.clear();
}

BLYNK_WRITE(V2) {
  // Add your code here to act upon Message change
  bool isOn = param.asInt() == 1;

  if (isOn) {
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }
}

float readCurrentWithCheck(ACS712 &sensor) {
  const int NUM_READINGS = 10;
  float readings[NUM_READINGS];

  // Take 10 readings
  for (int i = 0; i < NUM_READINGS; i++) {
    readings[i] = sensor.getCurrentAC();
    delay(10);  // Small delay between readings
  }

  // Check if any reading is close to zero
  for (int i = 0; i < NUM_READINGS; i++) {
    if (readings[i] <= 0.15) {
      return 0;
    }
  }

  // Calculate average of readings
  float sum = 0;
  for (int i = 0; i < NUM_READINGS; i++) {
    sum += readings[i];
  }
  return sum / NUM_READINGS;
}

void sendSMS(String message) {
  sim800.println("AT+CMGF=1");  // Set SMS mode to text
  delay(500);
  sim800.println("AT+CMGS=\"+2348029693169\"");  // Replace with your phone number
  delay(500);
  sim800.print(message);
  sim800.write(26);  // Ctrl+Z to send SMS
  delay(2000);
}

// -------------------- 🔹 NEW FUNCTION: SMS Command Handling --------------------
void checkForIncomingSMS() {
  if (sim800.available()) {
    String smsData = sim800.readString();
    smsData.toUpperCase();  // normalize

    if (smsData.indexOf("POWER") >= 0) {
      sendSMS("Current Power: " + String(realPower, 2) + " W");
      lcd.clear();
      lcd.print(F("Sending Status"));
      delay(3000);
      lcd.clear();
    } else if (smsData.indexOf("TURN ON") >= 0) {
      digitalWrite(RELAY_PIN, HIGH);
      sendSMS("Socket turned ON");
    } else if (smsData.indexOf("TURN OFF") >= 0) {
      digitalWrite(RELAY_PIN, LOW);
      sendSMS("Socket turned OFF");
    } else if (smsData.indexOf("LIMIT") >= 0) {
      int index = smsData.indexOf("LIMIT");
      String num = smsData.substring(index + 5);
      num.trim();
      int newLimit = num.toInt();
      if (newLimit > 0) {
        energyLimit = newLimit;
        sendSMS("Energy limit set to " + String(energyLimit) + " W");
        Blynk.virtualWrite(V1, energyLimit);
      } else {
        sendSMS("Invalid LIMIT value");
      }
    } else if (smsData.indexOf("HELP") >= 0) {
      String helpMsg = "Commands:\n"
                       "POWER - Get power usage\n"
                       "TURN ON - Turn relay ON\n"
                       "TURN OFF - Turn relay OFF\n"
                       "LIMIT 200 - Set limit\n"
                       "HELP - Show commands";
      sendSMS(helpMsg);
    }
    while (sim800.available()) sim800.read();
  }
}
