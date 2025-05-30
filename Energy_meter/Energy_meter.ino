#include <ACS712.h>
#include <ZMPT101B.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#include "commands.h"

#define ACTUAL_VOLTAGE 220.0f
#define TOLLERANCE 3.0f
#define MAX_TOLLERANCE_VOLTAGE (ACTUAL_VOLTAGE + TOLLERANCE)
#define MIN_TOLLERANCE_VOLTAGE (ACTUAL_VOLTAGE - TOLLERANCE)

#define FAST_STEP 5.0f
#define FINE_STEP 0.25f

#define VOLTAGE_SENSOR_PIN A0
#define CURRENT_SENSOR_PIN_1 A1
#define CURRENT_SENSOR_PIN_2 A2

#define ESP_SERIAL Serial

#define RELAY_PIN 2
#define RELAY_PIN_2 A3

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 5, 6, 7, 8 };     //connect to the row pinouts of the keypad
byte colPins[COLS] = { 9, 10, 11, 12 };  //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 4);

ACS712 current_sensor_1(ACS712_05B, CURRENT_SENSOR_PIN_1);
ACS712 current_sensor_2(ACS712_05B, CURRENT_SENSOR_PIN_2);

ZMPT101B voltageSensor(VOLTAGE_SENSOR_PIN, 50.0);

float balance = 0;
float energyConsumed = 0;
float balance2 = 0;
float energyConsumed2 = 0;
unsigned long lastEnergyUpdate = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);

  // digitalWrite(RELAY_PIN, HIGH);
  // digitalWrite(RELAY_PIN_2, HIGH);

  ESP_SERIAL.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.print(F("Calibrating"));
  lcd.setCursor(0, 1);
  lcd.print(F("System"));

  for (int i = 0; i < 50; i++) {
    current_sensor_1.calibrate();
    current_sensor_2.calibrate();
    delay(100);
  }

  float sensitivity = 0.0f;
  float voltageNow;

  while (sensitivity <= 1000.0f) {
    voltageSensor.setSensitivity(sensitivity);
    voltageNow = voltageSensor.getRmsVoltage();

    if (voltageNow >= MIN_TOLLERANCE_VOLTAGE && voltageNow <= MAX_TOLLERANCE_VOLTAGE) {
      break;
    }

    sensitivity += FAST_STEP;
  }

  if (sensitivity > 1000.0f) {
    lcd.clear();
    lcd.print(F("Failed to"));
    lcd.setCursor(0, 1);
    lcd.print(F("calibrate"));
    while (true)
      ;
  }

  float fineSensitivity = sensitivity - FAST_STEP;
  if (fineSensitivity < 0) fineSensitivity = 0;

  while (fineSensitivity <= sensitivity + FAST_STEP) {
    voltageSensor.setSensitivity(fineSensitivity);
    voltageNow = voltageSensor.getRmsVoltage();

    if (voltageNow >= MIN_TOLLERANCE_VOLTAGE && voltageNow <= MAX_TOLLERANCE_VOLTAGE) {
      lcd.clear();
      return;
    }

    fineSensitivity += FINE_STEP;
  }

  lcd.clear();
  lcd.print(F("Failed to"));
  lcd.setCursor(0, 1);
  lcd.print(F("calibrate"));
  while (true)
    ;
}


void loop() {
  // put your main code here, to run repeatedly:

  char key = keypad.getKey();

  if (key == 'D') {
    String value = "";
    lcd.clear();
    lcd.print(F("Set Balance 1:"));
    while (true) {
      char key = keypad.getKey();

      if (key >= '0' && key <= '9') {
        value += key;
      }

      else if (key == '*') {
        value = "";
        lcd.clear();
        lcd.print(F("Set Balance 1:"));
      }

      else if (key == '#') {
        balance = value.toInt();
        sendCommand(CMD_BALANCE, balance);
        value = "";
        break;
      }

      lcd.setCursor(0, 1);
      lcd.print(value);
    }

    lcd.clear();
    lcd.print(F("Set Balance 2:"));
    while (true) {
      char key = keypad.getKey();

      if (key >= '0' && key <= '9') {
        value += key;
      }

      else if (key == '*') {
        value = "";
        lcd.clear();
        lcd.print(F("Set Balance 2:"));
      }

      else if (key == '#') {
        balance2 = value.toInt();
        sendCommand(CMD_BALANCE_2, balance2);
        break;
      }

      lcd.setCursor(0, 1);
      lcd.print(value);
    }
  }

  if (ESP_SERIAL.available()) {
    String command = ESP_SERIAL.readStringUntil('\n');

    processCommand(command);
  }

  float Vrms = voltageSensor.getRmsVoltage();
  float Irms = readCurrentWithCheck(current_sensor_1);
  float Irms2 = readCurrentWithCheck(current_sensor_2);
  float realPower = Vrms * Irms;
  float realPower2 = Vrms * Irms2;

  // Energy calculation
  // Energy update
  unsigned long now = millis();
  float deltaTimeHours = (now - lastEnergyUpdate) / 3600000.0;
  lastEnergyUpdate = now;

  float energyThisLoop = realPower * deltaTimeHours;
  float energyThisLoop2 = realPower2 * deltaTimeHours;

  energyConsumed += energyThisLoop;
  balance -= energyThisLoop;

  energyConsumed2 += energyThisLoop2;
  balance2 -= energyThisLoop2;

  if (balance <= 0) {
    digitalWrite(RELAY_PIN, LOW);  // Turn off power
    balance = 0;
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // Keep power on
  }

  if (balance2 <= 0) {
    digitalWrite(RELAY_PIN_2, LOW);  // Turn off power
    balance2 = 0;
  } else {
    digitalWrite(RELAY_PIN_2, HIGH);  // Keep power on
  }

  lcd.setCursor(0, 0);
  lcd.print(F("B: "));
  lcd.print(balance);
  lcd.print(F(" Wh       "));
  sendCommand(CMD_BALANCE, balance);

  lcd.setCursor(0, 1);
  lcd.print(F("En: "));
  lcd.print(energyConsumed);
  lcd.print(F(" Wh       "));
  sendCommand(CMD_ENERGY, energyConsumed);
  lcd.setCursor(0, 2);
  lcd.print(F("B2: "));
  lcd.print(balance2);
  lcd.print(F(" Wh       "));
  sendCommand(CMD_BALANCE_2, balance2);

  lcd.setCursor(0, 3);
  lcd.print(F("En2: "));
  lcd.print(energyConsumed2);
  lcd.print(F(" Wh       "));
  sendCommand(CMD_ENERGY_2, energyConsumed2);

  delay(1000);
}

void sendCommand(String key, float value) {
  Serial.print(key);
  Serial.print("=");
  Serial.print(value, 2);  // send with 2 decimal precision
  Serial.print('\n');
}

void request(String key) {
  Serial.print(key);
  Serial.print('\n');
}

void processCommand(String cmd) {
  int sepIndex = cmd.indexOf('=');

  if (sepIndex != -1) {
    String key = cmd.substring(0, sepIndex);
    float value = cmd.substring(sepIndex + 1).toFloat();

    if (key == CMD_BALANCE) balance = value;
    else if (key == CMD_BALANCE_2) balance2 = value;
    else if (key == CMD_ENERGY) energyConsumed = value;
    else if (key == CMD_ENERGY_2) energyConsumed2 = value;
  } else {
    if (cmd == CMD_BALANCE) sendCommand(CMD_BALANCE, balance);
    else if (cmd == CMD_BALANCE_2) sendCommand(CMD_BALANCE_2, balance2);
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
