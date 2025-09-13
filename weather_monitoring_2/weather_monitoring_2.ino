#include <LiquidCrystal.h>
// #include <Adafruit_BMP085.h>
#include <DHT.h>

// Define pins
#define DHTPIN 2
#define DHTTYPE DHT11
#define LDR_PIN A0
#define RAIN_PIN A1

// Initialize objects
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);
DHT dht(DHTPIN, DHTTYPE);
// Adafruit_BMP085 bmp;

void setup() {
  lcd.begin(16, 2);
  dht.begin();
  
  // if (!bmp.begin()) {
  //   lcd.print("BMP180 missing");
  //   while (1);
  // }
  
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();

  lcd.print(F("WEATHER"));
  lcd.setCursor(0, 1);
  lcd.print(F("MONITORING SYS"));
  delay(3000);
  lcd.clear();
}

void loop() {
  // Read sensors
  float dhtTemp = dht.readTemperature();
  float humidity = dht.readHumidity();
  // float bmpTemp = bmp.readTemperature();
  // float pressure = bmp.readPressure() / 100.0; // hPa
  int ldrValue = analogRead(LDR_PIN);
  int rainValue = analogRead(RAIN_PIN);

  ldrValue = map(ldrValue, 0, 1023, 0, 100);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(dhtTemp, 1);
  lcd.setCursor(0, 1);
  lcd.print("Humid:");
  lcd.print(humidity, 0);
  lcd.print("%");

  delay(3000);

  // lcd.clear();
  // lcd.setCursor(0, 0);
  // lcd.print("Pres:");
  // lcd.print(pressure, 0);
  // lcd.print(" hPa");

  lcd.setCursor(0, 0);
  lcd.print("LDR:");
  lcd.print(ldrValue);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print(rainValue < 500 ? "Raining..." : "Not Raining");
  lcd.print("   ");

  delay(3000);
}
