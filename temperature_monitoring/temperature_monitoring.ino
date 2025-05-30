#include <LiquidCrystal.h>
#include <DHT.h>

// Pin Definitions
#define DHTPIN 2
#define DHTTYPE DHT11
#define BUZZER_PIN 9

#define BTN_UP 6
#define BTN_DOWN 7
#define BTN_SET 8

// LCD Pins (RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
DHT dht(DHTPIN, DHTTYPE);

// Variables
int desiredTemp = 25;
bool settingMode = true;

void setup() {
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SET, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.begin(16, 2);
  dht.begin();

  lcd.print("Temp Monitor");
  delay(1500);
  lcd.clear();
}

void loop() {
  float temp = dht.readTemperature();

  // Check button presses
  if (digitalRead(BTN_SET) == LOW) {
    settingMode = !settingMode;
    delay(300); // debounce delay
  }

  if (settingMode) {
    lcd.setCursor(0, 0);
    lcd.print("Set Temp: ");
    lcd.print(desiredTemp);
    lcd.print("C  ");

    if (digitalRead(BTN_UP) == LOW) {
      desiredTemp++;
      delay(200);
    }
    if (digitalRead(BTN_DOWN) == LOW) {
      desiredTemp--;
      delay(200);
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Current: ");
    if (isnan(temp)) {
      lcd.print("Error");
    } else {
      lcd.print(temp);
      lcd.print("C ");
    }

    lcd.setCursor(0, 1);
    lcd.print("Target: ");
    lcd.print(desiredTemp);
    lcd.print("C  ");

    // Buzzer trigger
    if (!isnan(temp) && temp >= desiredTemp) {
      for (int i = 0; i < 3; i++) { // Beep 3 times
        digitalWrite(BUZZER_PIN, HIGH);
        delay(300);
        digitalWrite(BUZZER_PIN, LOW);
        delay(300);
      }
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }
  }

  delay(500);
}
