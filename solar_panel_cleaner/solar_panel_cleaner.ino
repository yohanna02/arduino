// Solar Panel Cleaner - Automatic (06:00, 18:00) + Clean Now mode
// Components: BTS7960 motor driver, 2x ultrasonic sensors, 16x2 I2C LCD, 4x3 Keypad, DS1307 RTC

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <RTClib.h>

// ---------- CONFIG ----------
LiquidCrystal_I2C lcd(0x27, 16, 2);  // change address if needed
RTC_DS1307 rtc;

// Motor (BTS7960) pins - adjust to your wiring
const int pwmR = 9;      // PWM for right input (RPWM)
const int pwmL = 10;     // PWM for left input (LPWM)
const int enableR = 11;  // R_EN (set HIGH)
const int enableL = 12;  // L_EN (set HIGH)

// Ultrasonic sensors (HC-SR04 style)
const int trigLeft = A2;
const int echoLeft = A3;
const int trigRight = A1;
const int echoRight = A0;

// Cleaning parameters
const int motorSpeed = 200;           // 0-255
const long endDistanceThreshold = 6;  // cm - distance to detect 'end' (adjust)

// Keypad setup (4x3)
const byte ROWS = 4;
const byte COLS = 3;
char keysArr[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
// row pins: use analog pins for convenience
byte rowPins[ROWS] = { 2, 3, 4, 5 };
byte colPins[COLS] = { 6, 7, 8 };
Keypad keypad = Keypad(makeKeymap(keysArr), rowPins, colPins, ROWS, COLS);

// State
enum Mode { MODE_IDLE,
            MODE_AUTOMATIC,
            MODE_CLEAN_NOW,
            MODE_SET_TIME };
Mode currentMode = MODE_IDLE;

int lastAutoRunDayFor6 = -1;   // day of month when 06:00 run executed
int lastAutoRunDayFor18 = -1;  // day of month when 18:00 run executed

int time1[2] = { 6, 0 };
int time2[2] = { 18, 0 };

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  // Motor pins
  pinMode(pwmR, OUTPUT);
  pinMode(pwmL, OUTPUT);
  pinMode(enableR, OUTPUT);
  pinMode(enableL, OUTPUT);
  digitalWrite(enableR, HIGH);
  digitalWrite(enableL, HIGH);
  stopMotor();

  // Ultrasonic pins
  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);
  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);

  // LCD and RTC
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Solar Cleaner");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(1200);

  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("RTC not found!");
    Serial.println("RTC not found!");
    while (1)
      ;  // stop - RTC required for auto mode; remove if you want to allow without RTC
  }

  if (!rtc.isrunning()) {
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // optionally set from compile time
    // If DS1307 not running, user should set time externally; we assume RTC is set.
    Serial.println("RTC not running");
  }

  lcd.clear();

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("Construction"));
  delay(3000);
  lcd.clear();

  lcd.print(F("of automatic"));
  lcd.setCursor(0, 1);
  lcd.print(F("solar panel"));
  delay(3000);
  lcd.clear();

  lcd.print(F("cleaning"));
  lcd.setCursor(0, 1);
  lcd.print(F("system"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Sadam Longji Bage"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151651"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Gyang George Davou"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151842"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr Tech.Dahiru Zalani"));
  delay(3000);
  lcd.clear();
}

// ---------- MAIN LOOP ----------
void loop() {
  // display time & prompt
  DateTime now = rtc.now();
  displayTimeAndMode(now);

  // check keypad
  char k = keypad.getKey();
  if (k) handleKey(k);

  // check automatic schedule
  if (currentMode == MODE_AUTOMATIC) {
    // run at 06:00 and 18:00 (6 and 18 hours) exactly once per day per slot
    int h = now.hour();
    int m = now.minute();
    int d = now.day();

    if (h == time1[0] && m == time1[1] && lastAutoRunDayFor6 != d) {
      lcd.clear();
      lcd.print("Auto run 06:00");
      Serial.println("Auto run 06:00");
      runCleaningRoutine();
      lastAutoRunDayFor6 = d;
    }
    if (h == time2[0] && m == time2[1] && lastAutoRunDayFor18 != d) {
      lcd.clear();
      lcd.print("Auto run 18:00");
      Serial.println("Auto run 18:00");
      runCleaningRoutine();
      lastAutoRunDayFor18 = d;
    }
  }

  // small delay to avoid spamming
  delay(300);
}

// ---------- HANDLERS ----------
void handleKey(char k) {
  Serial.print("Key: ");
  Serial.println(k);
  if (k == '1') {
    currentMode = MODE_AUTOMATIC;
    lcd.clear();
    lcd.print("Mode: Automatic");
    lcd.setCursor(0, 1);
    lcd.print("6:00 & 18:00");
    delay(1000);
  } else if (k == '2') {
    currentMode = MODE_CLEAN_NOW;
    lcd.clear();
    lcd.print("Mode: Clean Now");
    lcd.setCursor(0, 1);
    lcd.print("Starting...");
    delay(600);
    runCleaningRoutine();
    // after clean now, return to idle
    currentMode = MODE_IDLE;
  } else if (k == '3') {
    currentMode = MODE_IDLE;
    lcd.clear();
    lcd.print("Mode: Idle");
    delay(600);
  } else if (k == '#') {
    // Optional: '#' could start immediate clean in any mode
    lcd.clear();
    lcd.print("Manual Start");
    delay(300);
    runCleaningRoutine();
  } else if (k == '4') {
    forward(motorSpeed);
  } else if (k == '5') {
    backward(motorSpeed);
  } else if (k == '6') {
    stopMotor();
  } else if (k == '*') {
    currentMode = MODE_SET_TIME;
    setAutomaticTimes();
    currentMode = MODE_IDLE;
  } else if (k == '9') {
    setRTCManually();
  }
}

void setAutomaticTimes() {
  lcd.clear();
  lcd.print("Set Auto Times");
  delay(1000);

  for (int t = 0; t < 2; t++) {
    int *target = (t == 0) ? time1 : time2;
    lcd.clear();
    lcd.print(t == 0 ? "Time1 (HHMM):" : "Time2 (HHMM):");

    String input = "";
    while (true) {
      char key = keypad.getKey();
      if (!key) continue;

      if (key >= '0' && key <= '9') {
        if (input.length() < 4) {
          input += key;
          lcd.setCursor(0, 1);
          lcd.print(input);
        }
      } else if (key == '*') {
        input = "";
        lcd.setCursor(0, 1);
        lcd.print("    ");
      } else if (key == '#') {
        if (input.length() == 4) {
          int h = input.substring(0, 2).toInt();
          int m = input.substring(2).toInt();
          if (h >= 0 && h < 24 && m >= 0 && m < 60) {
            target[0] = h;
            target[1] = m;
            lcd.clear();
            lcd.print("Saved: ");
            lcd.print(h);
            lcd.print(":");
            if (m < 10) lcd.print("0");
            lcd.print(m);
            delay(1000);
            break;
          } else {
            lcd.clear();
            lcd.print("Invalid Time!");
            delay(1000);
            lcd.clear();
            lcd.print("Try again:");
            input = "";
          }
        }
      }
    }
  }

  lcd.clear();
  lcd.print("Times Updated!");
  delay(1500);
  lcd.clear();
}

void setRTCManually() {
  lcd.clear();
  lcd.print("Set Clock (HHMM)");
  String input = "";
  while (true) {
    char key = keypad.getKey();
    if (!key) continue;

    if (key >= '0' && key <= '9' && input.length() < 4) {
      input += key;
      lcd.setCursor(0, 1);
      lcd.print(input);
    } else if (key == '*') {
      input = "";
      lcd.setCursor(0, 1);
      lcd.print("    ");
    } else if (key == '#') {
      if (input.length() == 4) {
        int h = input.substring(0, 2).toInt();
        int m = input.substring(2).toInt();
        if (h >= 0 && h < 24 && m >= 0 && m < 60) {
          DateTime now = rtc.now();
          rtc.adjust(DateTime(now.year(), now.month(), now.day(), h, m, 0));
          lcd.clear();
          lcd.print("RTC Updated");
          delay(1000);
          break;
        } else {
          lcd.clear();
          lcd.print("Invalid Time!");
          delay(1000);
          lcd.clear();
          lcd.print("Retry:");
          input = "";
        }
      }
    }
  }
  lcd.clear();
}

// ---------- CLEANING ROUTINE ----------
void runCleaningRoutine() {
  lcd.clear();
  lcd.print("Cleaning...");
  Serial.println("Cleaning started");
  unsigned long start = millis();

  // Move forward
  forward(motorSpeed);

  while (1) {
    long dL = getDistanceCM(trigLeft, echoLeft);
    long dR = getDistanceCM(trigRight, echoRight);
    Serial.print("L:");
    Serial.print(dL);
    Serial.print(" R:");
    Serial.println(dR);

    // update LCD with distances
    lcd.setCursor(0, 1);
    lcd.print("L:");
    lcd.print(dL >= 0 ? dL : 999);
    lcd.print(" R:");
    lcd.print(dR >= 0 ? dR : 999);

    // if either sensor reports an 'end' (object close)
    if (dR > 4 && dR <= endDistanceThreshold) {
      stopMotor();
      lcd.clear();
      lcd.print(F("End Reached"));
      break;
    }
    delay(200);  // sensor read interval
  }

  delay(3000);
  backward(motorSpeed);

  lcd.clear();
  lcd.print("Cleaning...");

  while (1) {
    long dL = getDistanceCM(trigLeft, echoLeft);
    long dR = getDistanceCM(trigRight, echoRight);
    Serial.print("L:");
    Serial.print(dL);
    Serial.print(" R:");
    Serial.println(dR);

    // update LCD with distances
    lcd.setCursor(0, 1);
    lcd.print("L:");
    lcd.print(dL >= 0 ? dL : 999);
    lcd.print(" R:");
    lcd.print(dR >= 0 ? dR : 999);

    // if either sensor reports an 'end' (object close)
    if (dL > 4 && dL <= endDistanceThreshold) {
      stopMotor();
      break;
    }
    delay(200);  // sensor read interval
  }


  lcd.clear();
  lcd.print("Done");
  Serial.println("Cleaning done");
  delay(1200);
  lcd.clear();
}

// ---------- MOTOR HELPERS ----------
void forward(int speed) {
  // One side PWM high, other 0 -> forward (module dependent, adjust if reversed)
  digitalWrite(enableR, HIGH);
  digitalWrite(enableL, HIGH);
  analogWrite(pwmR, speed);
  analogWrite(pwmL, 0);
}

void backward(int speed) {
  digitalWrite(enableR, HIGH);
  digitalWrite(enableL, HIGH);
  analogWrite(pwmR, 0);
  analogWrite(pwmL, speed);
}

void stopMotor() {
  analogWrite(pwmR, 0);
  analogWrite(pwmL, 0);
}

// ---------- ULTRASONIC ----------
long getDistanceCM(int trigPin, int echoPin) {
  // returns distance in cm or -1 on timeout
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned long duration = pulseIn(echoPin, HIGH, 30000UL);  // 30 ms timeout -> ~5 m
  if (duration == 0) return -1;
  long distanceCm = duration / 58;  // approximate
  return distanceCm;
}

// ---------- DISPLAY HELPERS ----------
void displayTimeAndMode(const DateTime &now) {
  lcd.setCursor(0, 0);
  char buf[17];
  sprintf(buf, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  lcd.print(buf);
  lcd.setCursor(0, 1);
  if (currentMode == MODE_AUTOMATIC) lcd.print("Automatic    ");
  else if (currentMode == MODE_CLEAN_NOW) lcd.print("Cleaning NOW    ");
  else lcd.print("IDEL            ");
}
