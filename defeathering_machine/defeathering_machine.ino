#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 0, A3, 3, 4 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 5, 6, 7, 8 };   //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define LOWER_LIMIT_SWITCH 1
#define UPPER_LIMIT_SWITCH A0

#define LIFTER_PIN_A A1
#define LIFTER_PIN_B A2

#define PUMP 9
#define BUZZER 10
#define HEATER 11
#define SPINNER 12

enum class MODE {
  SETTING,
  BOILING_WATER,
  WAITING_FOR_CHICKEN,
  PLACING_CHICKEN,
  DEFEATHERING
};

MODE current_mode = MODE::SETTING;

struct BuzzerControl {
  int duration;              // how long each beep lasts (ms)
  int interval;              // pause between beeps (ms)
  bool state;                // current buzzer state (on/off)
  unsigned long lastChange;  // last state change time
  bool active;               // is the buzzer currently active?
};

BuzzerControl buzzer = { 0, 0, false, 0, false };


int targetTemperature = 0;
int durationSeconds = 0;
int defeatheringDuration = 0;
unsigned long startMillis = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LOWER_LIMIT_SWITCH, INPUT_PULLUP);
  pinMode(UPPER_LIMIT_SWITCH, INPUT_PULLUP);

  pinMode(LIFTER_PIN_A, OUTPUT);
  pinMode(LIFTER_PIN_B, OUTPUT);

  pinMode(PUMP, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(SPINNER, OUTPUT);

  lcd.init();
  lcd.backlight();
  sensors.begin();
  lcd.print(F("Resetting...."));
}

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();
  if (key == 'C') {
    stopBeep();
    current_mode = MODE::SETTING;
    digitalWrite(SPINNER, LOW);
    digitalWrite(LIFTER_PIN_A, LOW);
    digitalWrite(LIFTER_PIN_B, LOW);
    digitalWrite(PUMP, LOW);
    lcd.clear();
    delay(1000);
    lcd.print(F("Resetting...."));
  }

  if (current_mode == MODE::SETTING) {
    while (digitalRead(LOWER_LIMIT_SWITCH)) {
      digitalWrite(LIFTER_PIN_A, LOW);
      digitalWrite(LIFTER_PIN_B, HIGH);
    }
    digitalWrite(LIFTER_PIN_A, LOW);
    digitalWrite(LIFTER_PIN_B, LOW);
    lcd.setCursor(0, 0);
    lcd.print(F("System Ready      "));
    lcd.setCursor(0, 1);
    lcd.print(F("Press # to start"));
    char key = keypad.getKey();

    while (key != '#') {
      key = keypad.getKey();
    }
    // set temperature and duration in seconds
    setTemperatureAndDuration();
    current_mode = MODE::BOILING_WATER;
  }

  if (current_mode == MODE::BOILING_WATER) {
    digitalWrite(HEATER, HIGH);
    sensors.requestTemperatures();

    float tempC = sensors.getTempCByIndex(0);
    lcd.setCursor(0, 3);
    if (tempC != DEVICE_DISCONNECTED_C) {
      lcd.print(F("Temp: "));
      lcd.print(tempC);
      lcd.print(F(" C   "));
    } else {
      lcd.print("Temp: Error    ");
    }

    if (tempC >= targetTemperature) {
      digitalWrite(HEATER, LOW);
      current_mode = MODE::WAITING_FOR_CHICKEN;
      startBeep(500, 500);
      lcd.clear();
    }
  }

  if (current_mode == MODE::WAITING_FOR_CHICKEN) {
    lcd.setCursor(0, 0);
    lcd.print(F("Press # to"));
    lcd.setCursor(0, 1);
    lcd.print(F("continue"));
    updateBuzzer();

    char key = keypad.getKey();

    if (key == '#') {
      lcd.clear();
      current_mode = MODE::PLACING_CHICKEN;
      startMillis = millis();
      lcd.print(F("Set duration:"));
      lcd.print(durationSeconds);
      lcd.print(F("s"));
      stopBeep();
    }
  }

  if (current_mode == MODE::PLACING_CHICKEN) {
    int current_time = (millis() - startMillis) / 1000;

    lcd.setCursor(0, 1);
    lcd.print(F("Time:"));
    lcd.print(current_time);
    lcd.print(F("s   "));

    if (current_time >= durationSeconds) {
      while (digitalRead(UPPER_LIMIT_SWITCH)) {
        digitalWrite(LIFTER_PIN_A, HIGH);
        digitalWrite(LIFTER_PIN_B, LOW);
      }
      digitalWrite(LIFTER_PIN_A, LOW);
      digitalWrite(LIFTER_PIN_B, LOW);
      current_mode = MODE::DEFEATHERING;
      startMillis = millis();
      lcd.clear();
      lcd.print(F("Set Time:"));
      lcd.print(defeatheringDuration);
    }
  }

  if (current_mode == MODE::DEFEATHERING) {
    int current_time = (millis() - startMillis) / 1000;
    digitalWrite(SPINNER, HIGH);
    digitalWrite(PUMP, HIGH);
    lcd.setCursor(0, 1);
    lcd.print(F("Time:"));
    lcd.print(current_time);
    lcd.print(F("s   "));

    if (current_time >= defeatheringDuration) {
      current_mode = MODE::SETTING;
      digitalWrite(SPINNER, LOW);
      digitalWrite(PUMP, LOW);
      lcd.clear();
    }
  }
}

void setTemperatureAndDuration() {
  // ================== TEMPERATURE ===================
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Temp (C):");
  lcd.setCursor(0, 1);

  String tempStr = "";
  while (true) {
    char k = keypad.getKey();
    if (k) {
      if (k >= '0' && k <= '9') {
        tempStr += k;
        lcd.print(k);
      } else if (k == 'A') {  // Confirm input
        targetTemperature = tempStr.toInt();
        break;
      } else if (k == 'B' && tempStr.length() > 0) {  // Backspace
        tempStr.remove(tempStr.length() - 1);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set Temp (C):");
        lcd.setCursor(0, 1);
        lcd.print(tempStr);
      } else if (k == '*') {  // Cancel
        return;
      }
    }
  }

  // ================== DURATION ===================
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Duration(s):");
  lcd.setCursor(0, 1);

  String durStr = "";
  while (true) {
    char k = keypad.getKey();
    if (k) {
      if (k >= '0' && k <= '9') {
        durStr += k;
        lcd.print(k);
      } else if (k == 'A') {  // Confirm input
        durationSeconds = durStr.toInt();
        break;
      } else if (k == 'B' && durStr.length() > 0) {  // Backspace
        durStr.remove(durStr.length() - 1);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set Duration(s):");
        lcd.setCursor(0, 1);
        lcd.print(durStr);
      } else if (k == '*') {  // Cancel
        return;
      }
    }
  }

  // ================== DEFEATHERING DURATION ===================
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Defeat(s):");
  lcd.setCursor(0, 1);

  String defeatherStr = "";
  while (true) {
    char k = keypad.getKey();
    if (k) {
      if (k >= '0' && k <= '9') {
        defeatherStr += k;
        lcd.print(k);
      } else if (k == 'A') {  // Confirm input
        defeatheringDuration = defeatherStr.toInt();
        break;
      } else if (k == 'B' && defeatherStr.length() > 0) {  // Backspace
        defeatherStr.remove(defeatherStr.length() - 1);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set Defeat(s):");
        lcd.setCursor(0, 1);
        lcd.print(defeatherStr);
      } else if (k == '*') {  // Cancel
        return;
      }
    }
  }

  // ================== SUMMARY ===================
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(targetTemperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(durationSeconds);
  lcd.print("s");

  lcd.setCursor(0, 2);
  lcd.print("Defeat: ");
  lcd.print(defeatheringDuration);
  lcd.print("s");

  delay(2500);
}

void startBeep(int duration, int interval) {
  buzzer.duration = duration;
  buzzer.interval = interval;
  buzzer.state = false;
  buzzer.lastChange = millis();
  buzzer.active = true;
}

void stopBeep() {
  buzzer.active = false;
  digitalWrite(BUZZER, LOW);
}

void updateBuzzer() {
  if (!buzzer.active) return;

  unsigned long now = millis();

  if (buzzer.state) {
    // Buzzer is ON
    if (now - buzzer.lastChange >= (unsigned long)buzzer.duration) {
      digitalWrite(BUZZER, LOW);
      buzzer.state = false;
      buzzer.lastChange = now;
    }
  } else {
    // Buzzer is OFF
    if (now - buzzer.lastChange >= (unsigned long)buzzer.interval) {
      digitalWrite(BUZZER, HIGH);
      buzzer.state = true;
      buzzer.lastChange = now;
    }
  }
}
