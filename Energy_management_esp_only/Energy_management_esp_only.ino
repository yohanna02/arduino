#define BLYNK_TEMPLATE_ID "TMPL2Iqf2LgLJ"
#define BLYNK_TEMPLATE_NAME "Smart Energy Management"
#define BLYNK_AUTH_TOKEN "pt0c0YacKiHJ7xyzzR7LOcnVHGhacW3q"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include "ACS712.h"
#include <ZMPT101B.h>

char ssid[] = "TECNO POP 8";
char pass[] = "werrrry123";

#define ACTUAL_VOLTAGE 220.0f
#define TOLLERANCE 3.0f
#define MAX_TOLLERANCE_VOLTAGE (ACTUAL_VOLTAGE + TOLLERANCE)
#define MIN_TOLLERANCE_VOLTAGE (ACTUAL_VOLTAGE - TOLLERANCE)

#define FAST_STEP 5.0f
#define FINE_STEP 0.25f

#define VOLTAGE_SENSOR_PIN 35
#define CURRENT_SENSOR_PIN_1 34
#define CURRENT_SENSOR_PIN_2 39

#define RELAY_PIN_1 19
#define RELAY_PIN_2 23

LiquidCrystal_I2C lcd(0x27, 20, 4);

ACS712 current_sensor_1(ACS712_05B, CURRENT_SENSOR_PIN_1);
ACS712 current_sensor_2(ACS712_05B, CURRENT_SENSOR_PIN_2);
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

float energyLimit = 0;

unsigned long prevMillis = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  lcd.print(F("Calibrating"));
  lcd.setCursor(0, 1);
  lcd.print(F("System"));

  current_sensor_1.calibrate();
  current_sensor_2.calibrate();

  lcd.clear();

  // float sensitivity = 0.0f;
  // float voltageNow;

  // while (sensitivity <= 1000.0f) {
  //   voltageSensor.setSensitivity(sensitivity);
  //   voltageNow = voltageSensor.getRmsVoltage();

  //   if (voltageNow >= MIN_TOLLERANCE_VOLTAGE && voltageNow <= MAX_TOLLERANCE_VOLTAGE) {
  //     break;
  //   }

  //   sensitivity += FAST_STEP;
  // }

  // if (sensitivity > 1000.0f) {
  //   lcd.clear();
  //   lcd.print(F("Failed to"));
  //   lcd.setCursor(0, 1);
  //   lcd.print(F("calibrate"));
  //   while (true)
  //     ;
  // }

  // float fineSensitivity = sensitivity - FAST_STEP;
  // if (fineSensitivity < 0) fineSensitivity = 0;

  // while (fineSensitivity <= sensitivity + FAST_STEP) {
  //   voltageSensor.setSensitivity(fineSensitivity);
  //   voltageNow = voltageSensor.getRmsVoltage();

  //   if (voltageNow >= MIN_TOLLERANCE_VOLTAGE && voltageNow <= MAX_TOLLERANCE_VOLTAGE) {
  //     lcd.clear();
  //     return;
  //   }

  //   fineSensitivity += FINE_STEP;
  // }

  // lcd.clear();
  // lcd.print(F("Failed to"));
  // lcd.setCursor(0, 1);
  // lcd.print(F("calibrate"));
  // while (true)
  //   ;
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
        Blynk.virtualWrite(V2, energyLimit);
        value = "";
        break;
      }

      lcd.setCursor(0, 1);
      lcd.print(value);
    }
  }

  float Vrms = 220;
  // float Vrms = voltageSensor.getRmsVoltage();
  float Irms_1 = readCurrentWithCheck(current_sensor_1);
  float Irms_2 = readCurrentWithCheck(current_sensor_2);
  float realPower_1 = Vrms * Irms_1;
  float realPower_2 = Vrms * Irms_2;

  lcd.setCursor(0, 0);
  lcd.print(F("System Data"));

  lcd.setCursor(0, 1);
  lcd.print(F("Load 1: "));
  lcd.print(realPower_1);
  lcd.print(F("W     "));

  lcd.setCursor(0, 2);
  lcd.print(F("Load 2: "));
  lcd.print(realPower_2);
  lcd.print(F("W     "));

  lcd.setCursor(0, 3);
  if (energyLimit == 0) {
    lcd.print(F("No limit set      "));
  } else {
    lcd.print(F("Limit: "));
    lcd.print(energyLimit);
    lcd.print(F("W                 "));
  }

  if (millis() - prevMillis > 3000) {
    Blynk.virtualWrite(V0, realPower_1);
    Blynk.virtualWrite(V1, realPower_2);

    prevMillis = millis();
  }

  if (energyLimit != 0) {
    if ((realPower_1 + realPower_2) > energyLimit) {
      float maxPower = max(realPower_1, realPower_2);

      if (maxPower == realPower_1) {
        digitalWrite(RELAY_PIN_1, LOW);
      }

      if (maxPower == realPower_2) {
        digitalWrite(RELAY_PIN_2, LOW);
      }
    }
  }
}

BLYNK_WRITE(V2) {
  // Add your code here to act upon Message change
  energyLimit = param.asInt();
  lcd.clear();
  lcd.print(F("Energy Limit"));
  lcd.setCursor(0, 1);
  lcd.print(F("set"));
  delay(3000);
  lcd.clear();
}

BLYNK_WRITE(V3) {
  // Add your code here to act upon Message change
  bool isOn = param.asInt() == 1;

  if (isOn) {
    digitalWrite(RELAY_PIN_1, HIGH);
  } else {
    digitalWrite(RELAY_PIN_1, LOW);
  }
}

BLYNK_WRITE(V4) {
  // Add your code here to act upon Message change
  bool isOn = param.asInt() == 1;

  if (isOn) {
    digitalWrite(RELAY_PIN_2, HIGH);
  } else {
    digitalWrite(RELAY_PIN_2, LOW);
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
