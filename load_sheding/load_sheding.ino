#include <Wire.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include <Keypad.h>

// Initialize RTC and LCD
RTC_DS3231 rtc;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Keypad settings
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; 
byte colPins[COLS] = {6, A0, A1, A2}; 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Pins for the loads
const int load1Pin = A3;
const int load2Pin = A4;

// Time variables
int onTimeLoad1[2] = {0, 0}; // HH:MM for Load 1 ON
int offTimeLoad1[2] = {0, 0}; // HH:MM for Load 1 OFF
int onTimeLoad2[2] = {0, 0}; // HH:MM for Load 2 ON
int offTimeLoad2[2] = {0, 0}; // HH:MM for Load 2 OFF

// State variables
bool load1State = false;
bool load2State = false;

void setup() {
  pinMode(load1Pin, OUTPUT);
  pinMode(load2Pin, OUTPUT);
  digitalWrite(load1Pin, LOW);
  digitalWrite(load2Pin, LOW);

  lcd.begin(16, 2);
  if (!rtc.begin()) {
    lcd.print("RTC not found!");
    while (1);
  }

  lcd.print("Set Load Times:");
  delay(2000);
  lcd.clear();
  setLoadTimes();
}

void loop() {
  DateTime now = rtc.now();
  int currentHour = now.hour();
  int currentMinute = now.minute();

  // Check Load 1
  if (timeWithinRange(currentHour, currentMinute, onTimeLoad1, offTimeLoad1)) {
    if (!load1State) {
      digitalWrite(load1Pin, HIGH);
      load1State = true;
      lcd.setCursor(0, 0);
      lcd.print("Load 1: ON      ");
    }
  } else {
    if (load1State) {
      digitalWrite(load1Pin, LOW);
      load1State = false;
      lcd.setCursor(0, 0);
      lcd.print("Load 1: OFF     ");
    }
  }

  // Check Load 2
  if (timeWithinRange(currentHour, currentMinute, onTimeLoad2, offTimeLoad2)) {
    if (!load2State) {
      digitalWrite(load2Pin, HIGH);
      load2State = true;
      lcd.setCursor(0, 1);
      lcd.print("Load 2: ON      ");
    }
  } else {
    if (load2State) {
      digitalWrite(load2Pin, LOW);
      load2State = false;
      lcd.setCursor(0, 1);
      lcd.print("Load 2: OFF     ");
    }
  }

  delay(1000);
}

// Helper function to check if the current time is within a range
bool timeWithinRange(int hour, int minute, int onTime[], int offTime[]) {
  int currentTime = hour * 60 + minute;
  int onTimeMinutes = onTime[0] * 60 + onTime[1];
  int offTimeMinutes = offTime[0] * 60 + offTime[1];

  return currentTime >= onTimeMinutes && currentTime < offTimeMinutes;
}

// Function to set load times
void setLoadTimes() {
  lcd.clear();
  lcd.print("Set Load 1 ON:");
  setTime(onTimeLoad1);

  lcd.clear();
  lcd.print("Set Load 1 OFF:");
  setTime(offTimeLoad1);

  lcd.clear();
  lcd.print("Set Load 2 ON:");
  setTime(onTimeLoad2);

  lcd.clear();
  lcd.print("Set Load 2 OFF:");
  setTime(offTimeLoad2);

  lcd.clear();
  lcd.print("Times Set!");
  delay(2000);
}

// Function to input time from keypad
void setTime(int timeArray[]) {
  char buffer[5] = "";
  int index = 0;

  while (index < 4) {
    char key = keypad.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {
        buffer[index++] = key;
        lcd.setCursor(index - 1, 1);
        lcd.print(key);
      }
    }
  }

  timeArray[0] = (buffer[0] - '0') * 10 + (buffer[1] - '0'); // Hours
  timeArray[1] = (buffer[2] - '0') * 10 + (buffer[3] - '0'); // Minutes
}
