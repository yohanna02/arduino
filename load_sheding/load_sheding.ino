#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Keypad.h>

// Initialize RTC and LCD
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad settings
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { A0, A1, A2, A3 };
byte colPins[COLS] = { 0, 1, 2, 3 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Pins for the relays
const int relay1Pin = 9;   // Pair 1, Relay 1
const int relay2Pin = 11;  // Pair 1, Relay 2
const int relay3Pin = 10;  // Pair 2, Relay 3
const int relay4Pin = 12;  // Pair 2, Relay 4

// Time variables for the relay pairs
int onTimePair1[2] = { 0, 0 };   // HH:MM for Pair 1 ON
int offTimePair1[2] = { 0, 0 };  // HH:MM for Pair 1 OFF
int onTimePair2[2] = { 0, 0 };   // HH:MM for Pair 2 ON
int offTimePair2[2] = { 0, 0 };  // HH:MM for Pair 2 OFF

// State variables
bool pair1State = false;
bool pair2State = false;

void setup() {
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(relay4Pin, OUTPUT);

  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);
  digitalWrite(relay3Pin, LOW);
  digitalWrite(relay4Pin, LOW);

  lcd.init();
  lcd.backlight();

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("construction of"));
  delay(3000);
  lcd.clear();

  lcd.print(F("a microcontroller"));
  lcd.setCursor(0, 1);
  lcd.print(F("based load"));
  delay(3000);
  lcd.clear();

  lcd.print(F("shedding"));
  lcd.setCursor(0, 1);
  lcd.print(F("system"));
  delay(3000);
  lcd.clear();

  lcd.print(F("GBAZAN S.CALEB"));
  lcd.setCursor(0, 1);
  lcd.print(F("BPU/EEE/H/EEP/22/001"));
  delay(3000);
  lcd.clear();

  lcd.print(F("ABAH JANE ALADI"));
  lcd.setCursor(0, 1);
  lcd.print(F("AMEH JOHNSON AMEH"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Mrs HELEN ADOYI"));
  delay(3000);
  lcd.clear();

  if (!rtc.begin()) {
    lcd.print("RTC not found!");
    while (1)
      ;
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

  // Display current time
  lcd.setCursor(0, 0);  // First row, start at column 0
  lcd.print("Time: ");
  lcd.print(formatTime(currentHour, currentMinute));
  lcd.print("  ");  // Ensure leftover characters are cleared

  // Check Pair 1 (Relay 1 & Relay 2)
  bool pair1On = timeWithinRange(currentHour, currentMinute, onTimePair1, offTimePair1);
  updateRelayState(pair1On, relay1Pin, relay2Pin, pair1State);

  // Check Pair 2 (Relay 3 & Relay 4)
  bool pair2On = timeWithinRange(currentHour, currentMinute, onTimePair2, offTimePair2);
  updateRelayState(pair2On, relay3Pin, relay4Pin, pair2State);

  // Display statuses on the second row
  lcd.setCursor(0, 1);  // Second row, start at column 0
  lcd.print("L1: ");
  lcd.print(pair1State ? "ON " : "OFF");
  lcd.print("-L2: ");
  lcd.print(pair2State ? "ON " : "OFF");

  delay(1000);  // Update every second
}

void updateRelayState(bool shouldBeOn, int relayPin1, int relayPin2, bool &state) {
  if (shouldBeOn && !state) {
    digitalWrite(relayPin1, HIGH);
    digitalWrite(relayPin2, HIGH);
    state = true;
  } else if (!shouldBeOn && state) {
    digitalWrite(relayPin1, LOW);
    digitalWrite(relayPin2, LOW);
    state = false;
  }
}

// Function to format time as HH:MM
String formatTime(int hour, int minute) {
  char buffer[6];                                               // Buffer for "HH:MM"
  snprintf(buffer, sizeof(buffer), "%02d:%02d", hour, minute);  // Format with leading zeros
  return String(buffer);
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
  setTime(onTimePair1);

  lcd.clear();
  lcd.print("Set Load 1 OFF:");
  setTime(offTimePair1);

  lcd.clear();
  lcd.print("Set Load 2 ON:");
  setTime(onTimePair2);

  lcd.clear();
  lcd.print("Set Load 2 OFF:");
  setTime(offTimePair2);

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

  timeArray[0] = (buffer[0] - '0') * 10 + (buffer[1] - '0');  // Hours
  timeArray[1] = (buffer[2] - '0') * 10 + (buffer[3] - '0');  // Minutes
}
