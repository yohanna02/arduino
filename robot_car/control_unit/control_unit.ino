#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <ESPComm.h>
#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 rtc;

ESPComm controlUnit(Serial);

LiquidCrystal_I2C lcd(0x27, 20, 4);

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 9, 8, 7, 6 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 5, 4, 3, 2 };  //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

enum class DrivingMode {
  NONE,
  BLUETOOTH,
  AUTO
};

DrivingMode currentDrivingMode = DrivingMode::NONE;

void onReceive(String key, String value) {
  if (key == "MODE") {
    if (value == "AUTO") {
      currentDrivingMode = DrivingMode::AUTO;
    } else if (value == "BLUETOOTH") {
      currentDrivingMode = DrivingMode::BLUETOOTH;
    } else if (value == "NONE") {
      currentDrivingMode = DrivingMode::NONE;
    }
  } else if (key == "DRIVE") {
    lcd.setCursor(0, 3);
    if (value == "F") {
      lcd.print(F("Forward     "));
    } else if (value == "B") {
      lcd.print(F("Backward    "));
    } else if (value == "S") {
      lcd.print(F("Stopped     "));
    }
  }
}

void setup() {
  lcd.init();
  lcd.backlight();
  controlUnit.begin(9600);
  controlUnit.onCommand(onReceive);
  if (!rtc.begin()) {
    lcd.print(F("RTC Error"));
    delay(3000);
    lcd.clear();
  }
}

void loop() {
  controlUnit.loop();

  char key = keypad.getKey();

  if (key == '1') {
    lcd.clear();
    controlUnit.send("MODE", "AUTO");
  } else if (key == '2') {
    lcd.clear();
    controlUnit.send("MODE", "BLUETOOTH");
  } else if (key == 'A') {
    lcd.clear();
    controlUnit.send("MODE", "NONE");
  }

  if (currentDrivingMode == DrivingMode::NONE) {
    lcd.setCursor(0, 0);
    lcd.print(F("Select a Mode            "));
    lcd.setCursor(0, 1);
    lcd.print(F("1. Auto Mode             "));
    lcd.setCursor(0, 2);
    lcd.print(F("2. Bluetooth Mode        "));
  } else if (currentDrivingMode == DrivingMode::AUTO) {
    DateTime now;
    char time[] = "hh:mm:ss";
    char date[] = "DD/MM/YYYY";
    now.toString(time);
    now.toString(date);
    lcd.setCursor(0, 0);
    lcd.print(F("Mode: Auto               "));
    lcd.setCursor(0, 1);
    lcd.print(time);
    lcd.print(F("                  "));
    lcd.setCursor(0, 2);
    lcd.print(date);
    lcd.print(F("                  "));
  } else if (currentDrivingMode == DrivingMode::BLUETOOTH) {
    DateTime now;
    char time[] = "hh:mm:ss";
    char date[] = "DD/MM/YYYY";
    now.toString(time);
    now.toString(date);
    lcd.setCursor(0, 0);
    lcd.print(F("Mode: Bluetooth"));
    lcd.setCursor(0, 1);
    lcd.print(time);
    lcd.print(F("                  "));
    lcd.setCursor(0, 2);
    lcd.print(date);
    lcd.print(F("                  "));
  }
}