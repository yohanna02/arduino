#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ===========================
// Pin Definitions
// ===========================
#define DHTPIN      25
#define DHTTYPE     DHT11
#define FAN_PIN     26

// ===========================
// Objects
// ===========================
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===========================
// Temperature Settings
// ===========================
const float FAN_ON_TEMP  = 38.0;   // Turn fan ON at 30°C
const float FAN_OFF_TEMP = 32.0;   // Turn fan OFF below 28°C

bool fanState = false;

void setup() {
  dht.begin();

  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Automatic");
  lcd.setCursor(0, 1);
  lcd.print("temp & humd");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("monitoring");
  lcd.setCursor(0, 1);
  lcd.print("system");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("By Computer");
  lcd.setCursor(0,1);
  lcd.print("Engineering");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("students");
  delay(2000);
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHT11 Error!");
    delay(2000);
    return;
  }

  // Fan Control
  if (!fanState && temperature >= FAN_ON_TEMP) {
    fanState = true;
    digitalWrite(FAN_PIN, HIGH);
  }

  if (fanState && temperature <= FAN_OFF_TEMP) {
    fanState = false;
    digitalWrite(FAN_PIN, LOW);
  }

  // LCD Display
  lcd.clear();

  // First line
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print((char)223);
  lcd.print("C ");

  lcd.print("H:");
  lcd.print((int)humidity);
  lcd.print("%");

  // Second line
  lcd.setCursor(0, 1);
  lcd.print("Fan:");
  lcd.print(fanState ? "ON " : "OFF");

  lcd.print(" ");

  if (temperature >= FAN_ON_TEMP)
    lcd.print("HOT");
  else
    lcd.print("OK ");

  delay(2000);
}