#include <Wire.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include <Keypad.h>

// Initialize RTC and LCD
RTC_DS1307 rtc;
LiquidCrystal lcd(8, 7, A3, A2, A1, A0);

// Keypad settings
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 5, 0, 1, 2 };
byte colPins[COLS] = { 3, 4, 6 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// 🔹 3 Relay Pins (3 Loads)
const int relay1Pin = 9;
const int relay2Pin = 10;
const int relay3Pin = 11;

// Time variables
int onTimeLoad1[2] = { 0, 0 };
int offTimeLoad1[2] = { 0, 0 };

int onTimeLoad2[2] = { 0, 0 };
int offTimeLoad2[2] = { 0, 0 };

int onTimeLoad3[2] = { 0, 0 };
int offTimeLoad3[2] = { 0, 0 };

// States
bool load1State = false;
bool load2State = false;
bool load3State = false;

void setup() {

  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);

  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);
  digitalWrite(relay3Pin, LOW);

  lcd.begin(16, 2);

  showStartupMessage();

  if (!rtc.begin()) {
    lcd.print("RTC not found!");
    while (1)
      ;
  }

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  showCurrentTime();
  lcd.print("Set Load Times");
  delay(2000);
  lcd.clear();

  setLoadTimes();
}

void loop() {

  DateTime now = rtc.now();
  int currentHour = now.hour();
  int currentMinute = now.minute();

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(formatTime(currentHour, currentMinute));
  lcd.print(" ");

  // 🔹 Check if user wants to change time
  char key = keypad.getKey();
  if (key == '*') {
    showCurrentTime();
    setLoadTimes();
  }

  // Load 1
  bool load1On = timeWithinRange(currentHour, currentMinute, onTimeLoad1, offTimeLoad1);
  updateRelayState(load1On, relay1Pin, load1State);

  // Load 2
  bool load2On = timeWithinRange(currentHour, currentMinute, onTimeLoad2, offTimeLoad2);
  updateRelayState(load2On, relay2Pin, load2State);

  // Load 3
  bool load3On = timeWithinRange(currentHour, currentMinute, onTimeLoad3, offTimeLoad3);
  updateRelayState(load3On, relay3Pin, load3State);

  lcd.setCursor(8, 0);
  lcd.print("L1:");
  lcd.print(load1State ? "ON " : "OFF");


  lcd.setCursor(0, 1);
  lcd.print("L2:");
  lcd.print(load2State ? "ON " : "OFF");
  lcd.print(" L3:");
  lcd.print(load3State ? "ON " : "OFF");

  delay(1000);
}

void showStartupMessage() {

  lcd.clear();
  lcd.print("Design & Const.");
  lcd.setCursor(0, 1);
  lcd.print("Load Shedding");
  delay(3000);

  lcd.clear();
  lcd.print("System Project");
  delay(3000);

  lcd.clear();
  lcd.print("Name:");
  lcd.setCursor(0, 1);
  lcd.print("O. M. Olatunji");
  delay(3000);

  lcd.clear();
  lcd.print("Matric No:");
  lcd.setCursor(0, 1);
  lcd.print("0073");
  delay(3000);

  lcd.clear();
  lcd.print("Name:");
  lcd.setCursor(0, 1);
  lcd.print("Akor Terwase");
  delay(3000);

  lcd.clear();
  lcd.print("Alfred");
  lcd.setCursor(0, 1);
  lcd.print("Matric: 0067");
  delay(3000);

  lcd.clear();
  lcd.print("O. Abigail O.");
  lcd.setCursor(0, 1);
  lcd.print("Matric: 0061");
  delay(3000);

  lcd.clear();
  lcd.print("Supervisor:");
  lcd.setCursor(0, 1);
  lcd.print("Jacob Luka");
  delay(3000);

  lcd.clear();
}


void updateRelayState(bool shouldBeOn, int relayPin, bool &state) {
  if (shouldBeOn && !state) {
    digitalWrite(relayPin, HIGH);
    state = true;
  } else if (!shouldBeOn && state) {
    digitalWrite(relayPin, LOW);
    state = false;
  }
}

String formatTime(int hour, int minute) {
  char buffer[6];
  snprintf(buffer, sizeof(buffer), "%02d:%02d", hour, minute);
  return String(buffer);
}

bool timeWithinRange(int hour, int minute, int onTime[], int offTime[]) {
  int currentTime = hour * 60 + minute;
  int onTimeMinutes = onTime[0] * 60 + onTime[1];
  int offTimeMinutes = offTime[0] * 60 + offTime[1];

  return currentTime >= onTimeMinutes && currentTime < offTimeMinutes;
}

void setLoadTimes() {

  lcd.clear();
  lcd.print("Set L1 ON:");
  setTime(onTimeLoad1);

  lcd.clear();
  lcd.print("Set L1 OFF:");
  setTime(offTimeLoad1);

  lcd.clear();
  lcd.print("Set L2 ON:");
  setTime(onTimeLoad2);

  lcd.clear();
  lcd.print("Set L2 OFF:");
  setTime(offTimeLoad2);

  lcd.clear();
  lcd.print("Set L3 ON:");
  setTime(onTimeLoad3);

  lcd.clear();
  lcd.print("Set L3 OFF:");
  setTime(offTimeLoad3);

  lcd.clear();
  lcd.print("Times Set!");
  delay(2000);
}

void setTime(int timeArray[]) {

  char buffer[5] = "";
  int index = 0;

  lcd.setCursor(0, 1);
  lcd.print("                ");  // Clear second line
  lcd.setCursor(0, 1);

  while (true) {

    char key = keypad.getKey();

    if (key) {

      // 🔹 If number pressed
      if (key >= '0' && key <= '9') {

        if (index < 4) {
          buffer[index++] = key;
          lcd.print(key);
        }
      }

      // 🔹 If '*' pressed → CLEAR everything
      else if (key == '*') {

        index = 0;
        memset(buffer, 0, sizeof(buffer));

        lcd.setCursor(0, 1);
        lcd.print("                ");  // clear row
        lcd.setCursor(0, 1);
      }

      // 🔹 If '#' pressed → Confirm entry
      else if (key == '#') {

        if (index == 4) {

          int hour = (buffer[0] - '0') * 10 + (buffer[1] - '0');
          int minute = (buffer[2] - '0') * 10 + (buffer[3] - '0');

          // Basic validation
          if (hour < 24 && minute < 60) {

            timeArray[0] = hour;
            timeArray[1] = minute;

            break;  // Exit input loop
          } else {
            lcd.setCursor(0, 1);
            lcd.print("Invalid Time!");
            delay(1500);

            index = 0;
            memset(buffer, 0, sizeof(buffer));

            lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
          }
        }
      }
    }
  }
}

void showCurrentTime() {

  lcd.clear();
  DateTime now = rtc.now();

  lcd.print("Current Time:");
  lcd.setCursor(0, 1);
  lcd.print(formatTime(now.hour(), now.minute()));

  delay(3000);
  lcd.clear();
}
