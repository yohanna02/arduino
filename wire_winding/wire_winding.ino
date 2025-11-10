#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
const byte ROWS = 4;  //four rows
const byte COLS = 3;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 5, 6, 7, 8 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 9, 10, 11 };   //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

#define HALL_EFFECT 2
#define RELAY 3

volatile unsigned long count = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(RELAY, OUTPUT);
  lcd.init();
  lcd.backlight();

  attachInterrupt(digitalPinToInterrupt(HALL_EFFECT), countUp, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0, 0);
  lcd.print(F("Count: "));
  lcd.print(count);
  lcd.print(F("    "));

  char key = keypad.getKey();

  if (key == '*') {
    lcd.clear();
    count = 0;
  } else if (key == '#') {
    digitalWrite(RELAY, !digitalRead(RELAY));
    delay(1000);
  }
}

void countUp() {
  count++;
}
