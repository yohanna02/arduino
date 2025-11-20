#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 5, 6, 7, 8 };
byte colPins[COLS] = { 9, 10, 11 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

#define RELAY 3

unsigned long previousMillis = 0;
unsigned long interval = 1000;  // 1 second
unsigned long count = 0;
unsigned long targetCount = 0;

bool counting = false;
String input = "";

void setup() {
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  lcd.init();
  lcd.backlight();

  lcd.print(F("Automatic coil"));
  lcd.setCursor(0, 1);
  lcd.print(F("winding machine"));
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter count:");
}

void loop() {
  unsigned long currentMillis = millis();

  // Handle counting (non-blocking)
  if (counting && (currentMillis - previousMillis >= interval)) {
    previousMillis = currentMillis;
    count++;
    lcd.setCursor(0, 1);
    lcd.print("Count: ");
    lcd.print(count);
    lcd.print("/");
    lcd.print(targetCount);
    lcd.print("   ");

    if (count >= targetCount) {
      counting = false;
      digitalWrite(RELAY, LOW);
      lcd.setCursor(0, 0);
      lcd.print("Done!           ");
      lcd.setCursor(0, 1);
      lcd.print("Count: ");
      lcd.print(count);
      lcd.print("     ");
    }
  }

  // Handle keypad input anytime
  char key = keypad.getKey();
  if (key) {
    if (key == '*') {
      // Stop/reset
      counting = false;
      count = 0;
      targetCount = 0;
      digitalWrite(RELAY, LOW);
      input = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter count:");
      lcd.setCursor(0, 1);
      lcd.print("               ");
    } else if (key == '#') {
      // Start counting if input valid
      if (input.length() > 0) {
        targetCount = input.toInt();
        if (targetCount > 0) {
          count = 0;
          counting = true;
          digitalWrite(RELAY, HIGH);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Winding...");
        }
        input = "";
      }
    } else if (isDigit(key)) {
      // Build input number
      if (input.length() < 4) {
        input += key;
        lcd.setCursor(0, 1);
        lcd.print("Count: ");
        lcd.print(input);
        lcd.print("    ");
      }
    }
  }
}
