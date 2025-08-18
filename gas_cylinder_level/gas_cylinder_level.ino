#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <ESPComm.h>

SoftwareSerial _serial(3, 4);
LiquidCrystal_I2C lcd(0x27, 16, 2);
ESPComm esp(Serial);

#define GSM_SERIAL _serial

#define DT 2
#define SCK 13
#define GAS_PIN A0
#define BUZZER_PIN A1

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 12, 11, 10, 9 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 8, 7, 6, 5 };     //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  // put your setup code here, to run once:
  esp.begin(9600);
  GSM_SERIAL.begin(9600);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GAS_PIN, INPUT);

  lcd.init();
  lcd.backlight();
}

void loop() {
  // put your main code here, to run repeatedly:
}
