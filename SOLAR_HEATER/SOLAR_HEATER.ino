#include <LiquidCrystal.h>
#include <Keypad.h>
#include <DHT.h>
#define DHTPIN 7
#define DHTTYPE DHT11
#define HEATER_ONE 9
#define FAN_ONE 10
#define DELAY 3000
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal lcd(1, 2, 3, 4, 5, 6);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

byte rowPins[ROWS] = { 11, 12, 13, A0 };
byte colPins[COLS] = { A1, A2, A3 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int setTemprature = 0;
float tempratureRead = 0;

bool setValue = true;
String value = "";

void setup() {
  dht.begin();
  lcd.begin(16, 2);  // ✅ changed from 20x4 to 16x2
  lcd.print(F("Set Temp"));
  pinMode(FAN_ONE, OUTPUT);
  pinMode(HEATER_ONE, OUTPUT);
}

void loop() {
  char key = keypad.getKey();
  if (key == '*') {
    setValue = true;
    lcd.clear();
    lcd.print(F("Setting Temp"));
    delay(2000);
    lcd.clear();
    lcd.print(F("Set Temp"));
  }

  while (setValue) {
    turnHeatersOn_Off(false);
    char key = keypad.getKey();
    if (key >= '0' && key <= '9') {
      if (value.length() <= 2) {
        value += key;
      }
      lcd.clear();
      lcd.print("Temp: " + String(value));
      delay(200);
    }

    else if (key == '*') {
      value = "";
      lcd.clear();
      lcd.print("Temp: " + String(value));
      delay(200);
    }

    else if (key == '#') {
      if (value.length() == 0) {
        lcd.clear();
        lcd.print("Empty value!");
      } else {
        lcd.clear();
        lcd.print("Set to:");
        lcd.setCursor(0, 1);
        lcd.print(value + "C");
        setTemprature = value.toInt();
        value = "";
        setValue = false;
        delay(2000);
      }
      delay(DELAY / 2);
    }
  }


  tempratureRead = dht.readTemperature();
  if (isnan(tempratureRead)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp Error!");
    lcd.setCursor(0, 1);
    lcd.print("Retrying...");
    delay(DELAY);
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T: " + String(tempratureRead, 1) + "C");
  lcd.setCursor(0, 1);
  lcd.print("Set: " + String(setTemprature) + "C");

  turnFansOn_Off(true);
  if (tempratureRead > setTemprature) {
    turnHeatersOn_Off(false);
  } else if (tempratureRead <= setTemprature - 2) {
    turnHeatersOn_Off(true);
  }
  delay(DELAY);
}

void turnHeatersOn_Off(bool isOn) {
  digitalWrite(HEATER_ONE, isOn ? HIGH : LOW);
}

void turnFansOn_Off(bool isOn) {
  digitalWrite(FAN_ONE, isOn ? HIGH : LOW);
}

void keypadEvent(KeypadEvent key) {
  switch (keypad.getState()) {
    case PRESSED:
      break;
    case HOLD:
      break;
  }
}
