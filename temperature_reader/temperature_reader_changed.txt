#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

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

float min_temp = 2, max_temp = 8;
bool beep = false;

float humidity, temperature;

void setup() {
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
  lcd.print(F("National"));
  lcd.setCursor(0, 1);
  lcd.print(F("Hospital Abuja"));
  delay(3000);
  lcd.clear();

  digitalWrite(FAN, HIGH);
  delay(2000);
  lcd.print(F("Initializing..."));
  delay(300000);
}

void toggleLed() {
  digitalWrite(BUZZER_PIN, HIGH);
  for (int i = LED_PIN_1; i <= LED_PIN_6; i++) {
    digitalWrite(i, HIGH);
  }
  delay(100);  // Beep duration
  digitalWrite(BUZZER_PIN, LOW);
  for (int i = LED_PIN_1; i <= LED_PIN_6; i++) {
    digitalWrite(i, LOW);
  }
}

void loop() {
  // Read temperature and humidity
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();


  // Check if reading failed
  if (isnan(humidity) || isnan(temperature)) {
    lcd.setCursor(0, 0);
    lcd.print(F("Sensor Error!"));
    delay(2000);
    return;
  }

  lcd.clear();
  // Display readings on LCD
  lcd.setCursor(0, 0);
  lcd.print(F("Temp: "));
  lcd.print(temperature);
  lcd.print(F("C"));
  lcd.setCursor(0, 1);
  lcd.print(F("Hum: "));
  lcd.print(humidity);
  lcd.print(F("%"));

  if (temperature <= min_temp) {
    delay(2000);
    digitalWrite(COMPRESSOR, LOW);
    beep = false;
  } else if (temperature >= max_temp) {
    delay(2000);
    digitalWrite(COMPRESSOR, HIGH);
    beep = true;
  }

  if (beep) {
    toggleLed();
  }

  delay(4000);
}
