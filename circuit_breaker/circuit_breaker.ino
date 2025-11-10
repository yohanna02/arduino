/*
  Arduino Password-Based Circuit Breaker (3 Relays, Individual Passwords)
  ---------------------------------------------------------------
  - 4x3 Keypad for password input
  - 16x2 non-I2C LCD (LiquidCrystal)
  - 3 relays controlling separate circuits

  Behavior:
  - All relays are ON by default.
  - Press '1', '2', or '3' to select which relay to toggle OFF.
  - When turning OFF, user sets a temporary 4-digit password for that relay.
  - To turn the relay back ON, the same 4-digit password must be entered.
  - Passwords are stored only in RAM (not EEPROM) and are reset when power is lost.

  Pin mapping (adjust as needed):
  - Keypad rows:     R1 -> A0, R2 -> A1, R3 -> A2, R4 -> A3
  - Keypad columns:  C1 -> 2,  C2 -> 3,  C3 -> 4
  - LCD: RS -> 5, EN -> 6, D4 -> 7, D5 -> 8, D6 -> 9, D7 -> 10
  - Relays: RELAY1 -> 11, RELAY2 -> 12, RELAY3 -> 13
*/

#include <Keypad.h>
#include <LiquidCrystal.h>

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'C' }
};
byte rowPins[ROWS] = { 8, 12, 13, A0 };
byte colPins[COLS] = { A1, A2, A3, A4  };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal lcd(1, 2, 3, 4, 5, 6);

const int RELAY1 = 9;
const int RELAY2 = 10;
const int RELAY3 = 11;
const bool RELAY_ACTIVE_LOW = false;  // change if needed

struct RelayControl {
  int pin;
  bool state;
  bool hasPassword;
  char password[5];
};

RelayControl relays[3] = {
  { RELAY1, true, false, "" },
  { RELAY2, true, false, "" },
  { RELAY3, true, false, "" }
};

void setRelay(RelayControl &r, bool on) {
  int level = (on ^ RELAY_ACTIVE_LOW) ? HIGH : LOW;
  digitalWrite(r.pin, level);
  r.state = on;
}

String getDigits(int len, const char *prompt) {
  lcd.clear();
  lcd.print(prompt);
  lcd.setCursor(0, 1);
  String entered = "";
  while ((int)entered.length() < len) {
    char k = keypad.getKey();
    if (k) {
      if (k >= '0' && k <= '9') {
        entered += k;
        lcd.print('*');
      } else if (k == '#') {
        if (entered.length() > 0) {
          entered.remove(entered.length() - 1);
          lcd.setCursor(entered.length(), 1);
          lcd.print(' ');
          lcd.setCursor(entered.length(), 1);
        }
      }
    }
  }
  delay(200);
  return entered;
}

void setup() {
  lcd.begin(16, 2);
  lcd.print(F("The Federal"));
  lcd.setCursor(0, 1);
  lcd.print(F("Polytechic Bauchi"));
  delay(3000);
  lcd.clear();
  lcd.print(F("Department of"));
  lcd.setCursor(0, 1);
  lcd.print(F("electrical and"));
  delay(3000);
  lcd.clear();
  lcd.print(F("electronics"));
  lcd.setCursor(0, 1);
  lcd.print(F("engineering"));
  delay(3000);
  lcd.clear();
  lcd.print(F("Fatima"));
  lcd.setCursor(0, 1);
  lcd.print(F("Muhammad"));
  delay(3000);
  lcd.clear();
  for (int i = 0; i < 3; i++) {
    pinMode(relays[i].pin, OUTPUT);
    setRelay(relays[i], true);  // all ON by default
  }
  lcd.print("System Ready");
  lcd.setCursor(0, 1);
  lcd.print("Relays: ON");
  delay(1000);
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Press 1-3 to toggle   ");
  lcd.setCursor(0, 1);
  lcd.print("Relay state update    ");

  char key = keypad.getKey();
  if (key >= '1' && key <= '3') {
    int idx = key - '1';
    RelayControl &r = relays[idx];

    if (r.state) {
      // Turn OFF and set password
      String pass = getDigits(4, "Set 4-digit pass:");
      pass.toCharArray(r.password, 5);
      r.hasPassword = true;
      setRelay(r, false);
      lcd.clear();
      lcd.print("Relay ");
      lcd.print(key);
      lcd.setCursor(0, 1);
      lcd.print("Turned OFF");
      delay(1000);
    } else {
      // Turn ON if password matches
      if (!r.hasPassword) {
        lcd.clear();
        lcd.print("No password set");
        delay(1000);
        return;
      }
      String attempt = getDigits(4, "Enter password:");
      if (attempt == String(r.password)) {
        setRelay(r, true);
        lcd.clear();
        lcd.print("Relay ");
        lcd.print(key);
        lcd.setCursor(0, 1);
        lcd.print("Turned ON");
        r.hasPassword = false;  // clear password
      } else {
        lcd.clear();
        lcd.print("Wrong Password");
      }
      delay(1000);
    }
  }
}