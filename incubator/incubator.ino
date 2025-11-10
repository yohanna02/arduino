#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <DHT.h>
#include <RTClib.h>

#define DHTPIN 9
#define DHTTYPE DHT11

#define BULB_PIN A2
#define HEATER_PIN A0
#define FAN_PIN A1

#define MOTOR_IN1 11
#define MOTOR_IN2 12
#define MOTOR_ENA 10  // PWM speed control

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
RTC_DS1307 rtc;

// --- Keypad setup ---
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 8, 7, 6, 5 };
byte colPins[COLS] = { 4, 3, 2 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- Variables ---
float setTemp = 37.5;
float setHum = 55;
int totalDays = 21;
String eggType = "Chicken";

unsigned long lastTurnMillis = 0;
const unsigned long TURN_INTERVAL = 10000;  // 6 hours
// const unsigned long TURN_INTERVAL = 6UL * 60UL * 60UL * 1000UL;  // 6 hours
unsigned long lastDisplay = 0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  dht.begin();
  rtc.begin();

  pinMode(BULB_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT);

  digitalWrite(FAN_PIN, HIGH);  // fan always on

  lcd.setCursor(0, 0);
  lcd.print("Egg Incubator");
  delay(2000);
  selectEggType();
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (millis() - lastDisplay > 2000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(eggType);
    lcd.print(" D:");
    lcd.print(totalDays);
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(temp);
    lcd.print(" H:");
    lcd.print(hum);
    lastDisplay = millis();
  }

  controlTemperature(temp);
  checkEggTurning();

  // Decrease day count daily
  static DateTime lastDay = rtc.now();
  DateTime now = rtc.now();
  if (now.day() != lastDay.day()) {
    totalDays--;
    lastDay = now;
    if (totalDays <= 0) {
      stopTurning();
      lcd.clear();
      lcd.print("Hatching Phase");
      while (true)
        ;  // stop
    }
  }
}

void selectEggType() {
  lcd.clear();
  lcd.print("1-Chicken 2-Turkey");
  while (true) {
    char key = keypad.getKey();
    if (key == '1') {
      eggType = "Chicken";
      setTemp = 37.5;
      setHum = 55;
      totalDays = 21;
      break;
    } else if (key == '2') {
      eggType = "Turkey";
      setTemp = 37.5;
      setHum = 60;
      totalDays = 28;
      break;
    }
  }
  lcd.clear();
  lcd.print("Selected: ");
  lcd.print(eggType);
  delay(1500);
}

void controlTemperature(float temp) {
  if (isnan(temp)) return;
  if (temp < setTemp - 0.3) {
    digitalWrite(BULB_PIN, HIGH);
    digitalWrite(HEATER_PIN, HIGH);
  } else if (temp > setTemp + 0.3) {
    digitalWrite(BULB_PIN, LOW);
    digitalWrite(HEATER_PIN, LOW);
  }
}

void checkEggTurning() {
  if (millis() - lastTurnMillis >= TURN_INTERVAL) {
    turnEggCrate();
    lastTurnMillis = millis();
  }
}

void turnEggCrate() {
  lcd.clear();
  lcd.print("Turning Eggs...");
  analogWrite(MOTOR_ENA, 110);
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  delay(200);
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  delay(1000);
  lcd.clear();
}

void stopTurning() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
}
