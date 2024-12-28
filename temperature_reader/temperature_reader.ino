#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Keypad.h>
#include <EEPROM.h>

#define MIN_TEMP_ADDR 0  // Starting address for min_temp
#define MAX_TEMP_ADDR 4  // Starting address for max_temp (next float, 4 bytes)

const byte ROWS = 4;  //four rows
const byte COLS = 3;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 2, A0, A1, A3 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { A2, 3, 4 };       //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Define pins
#define DHTPIN 1  // Pin connected to DHT11
#define DHTTYPE DHT11
#define BUZZER_PIN 13  // Pin connected to buzzer

#define FAN 5
#define COMPRESSOR 6

#define LED_PIN_1 7   // Pin connected to LED
#define LED_PIN_2 8   // Pin connected to LED
#define LED_PIN_3 9   // Pin connected to LED
#define LED_PIN_4 10  // Pin connected to LED
#define LED_PIN_5 11  // Pin connected to LED
#define LED_PIN_6 12  // Pin connected to LED

// Initialize DHT sensor and LCD
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long prevMillis = 0, beepMillis = 0;

float min_temp, max_temp = 0;
bool set_temp = false, beep = false;

float humidity, temperature;

void set_value(float &value, int min_value, int max_value) {
  String temp_value;

  while (1) {
    char key = keypad.getKey();

    if (key >= '0' && key <= '9') {
      temp_value += key;
      lcd.setCursor(0, 1);
      lcd.print(temp_value);
    } else if (key == '*') {
      lcd.setCursor(0, 1);
      temp_value = "";
      lcd.print(F("                  "));
    } else if (key == '#') {
      int set_temp = temp_value.toFloat();

      if (set_temp >= min_value && set_temp <= max_value) {
        value = set_temp;

        // Save to EEPROM
        if (&value == &min_temp) {
          EEPROM.put(MIN_TEMP_ADDR, min_temp);
        } else if (&value == &max_temp) {
          EEPROM.put(MAX_TEMP_ADDR, max_temp);
        }

        lcd.clear();
        break;
      } else {
        lcd.setCursor(0, 1);
        lcd.print(F("Invalid Params"));
        delay(3000);
        lcd.print(F("                  "));
        temp_value = "";
      }
    }
  }
}

void setup() {
  // Read saved values from EEPROM
  EEPROM.get(MIN_TEMP_ADDR, min_temp);
  EEPROM.get(MAX_TEMP_ADDR, max_temp);

  // Validate saved values
  if (isnan(min_temp) || min_temp < 0 || min_temp > 50) min_temp = 0;          // Default if invalid
  if (isnan(max_temp) || max_temp < min_temp || max_temp > 50) max_temp = 50;  // Default if invalid

  // Initialize DHT sensor
  dht.begin();

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Setup buzzer and LED pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  pinMode(LED_PIN_5, OUTPUT);
  pinMode(LED_PIN_6, OUTPUT);

  pinMode(FAN, OUTPUT);
  pinMode(COMPRESSOR, OUTPUT);

  // Display startup message
  lcd.setCursor(0, 0);
  lcd.print("National");
  lcd.setCursor(0, 1);
  lcd.print("Hospital Abuja");
  delay(3000);
  lcd.clear();

  digitalWrite(FAN, HIGH);
  lcd.print(F("Initializing..."));
  delay(300000);
  lcd.clear();
  digitalWrite(COMPRESSOR, HIGH);
  beep = true;
}

void loop() {

  char key = keypad.getKey();
  if (key == '*') {
    set_temp = true;
    lcd.clear();
    digitalWrite(FAN, LOW);
    digitalWrite(COMPRESSOR, LOW);
  }

  if (set_temp) {
    lcd.clear();
    lcd.print(F("Set Max Temp"));
    set_value(max_temp, 0, 50);

    lcd.clear();
    lcd.print(F("Set min Temp"));
    set_value(min_temp, 0, max_temp);
    set_temp = false;
    digitalWrite(FAN, HIGH);
    lcd.print(F("Initializing..."));
    delay(300000);
    lcd.clear();
    digitalWrite(COMPRESSOR, HIGH);
    beep = true;
  }

  if (millis() - prevMillis > 2000) {
    // Read temperature and humidity
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    // Check if reading failed
    if (isnan(humidity) || isnan(temperature)) {
      lcd.setCursor(0, 0);
      lcd.print("Sensor Error!");
      return;
    }

    lcd.clear();
    // Display readings on LCD
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(humidity);
    lcd.print("%");

    if (temperature < min_temp) {
      digitalWrite(COMPRESSOR, LOW);
      beep = false;
    } else if (temperature > max_temp) {
      digitalWrite(COMPRESSOR, HIGH);
      beep = true;
    }
    prevMillis = millis();
  }

  if ((millis() - beepMillis > 1900) && beep) {
    // Beep buzzer and blink LED
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN_1, HIGH);
    digitalWrite(LED_PIN_2, HIGH);
    digitalWrite(LED_PIN_3, HIGH);
    digitalWrite(LED_PIN_4, HIGH);
    digitalWrite(LED_PIN_5, HIGH);
    digitalWrite(LED_PIN_6, HIGH);
    delay(100);  // Beep duration
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN_1, LOW);
    digitalWrite(LED_PIN_2, LOW);
    digitalWrite(LED_PIN_3, LOW);
    digitalWrite(LED_PIN_4, LOW);
    digitalWrite(LED_PIN_5, LOW);
    digitalWrite(LED_PIN_6, LOW);
    beepMillis = millis();
  }
}
