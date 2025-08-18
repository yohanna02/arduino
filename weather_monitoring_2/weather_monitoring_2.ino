#include <LiquidCrystal.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>

// Define pins
#define DHTPIN 9
#define DHTTYPE DHT11
#define LDR_PIN A0

// Initialize objects
LiquidCrystal lcd(1, 2, 3, 4, 5, 6);
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

void setup() {
  lcd.begin(16, 2);
  dht.begin();
  
  if (!bmp.begin()) {
    lcd.print("BMP180 missing");
    while (1);
  }
  
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();

  lcd.print(F("DESIGN AND"));
  lcd.setCursor(0, 1);
  lcd.print(F("CONSTRUCTION"));
  delay(3000);
  lcd.clear();

  lcd.print(F("OF WEATHER"));
  lcd.setCursor(0, 1);
  lcd.print(F("MONITORING DEVICE"));
  delay(3000);
  lcd.clear();

  lcd.print(F("OF WEATHER"));
  lcd.setCursor(0, 1);
  lcd.print(F("MONITORING DEVICE"));
  delay(3000);
  lcd.clear();

  lcd.print(F("AYUBA Y.OLATUNJI"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151833"));
  delay(3000);
  lcd.clear();

  lcd.print(F("SUPERVISED BY"));
  lcd.setCursor(0, 1);
  lcd.print(F("MR.DAUDA BUBA"));
  delay(3000);
  lcd.clear();
}

void loop() {
  // Read sensors
  float dhtTemp = dht.readTemperature();
  float humidity = dht.readHumidity();
  float bmpTemp = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0; // hPa
  int ldrValue = analogRead(LDR_PIN);

  ldrValue = map(ldrValue, 0, 1023, 0, 100);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(dhtTemp, 1);
  lcd.print(" H:");
  lcd.print(humidity, 0);
  lcd.print("%");

  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pres:");
  lcd.print(pressure, 0);
  lcd.print(" hPa");

  lcd.setCursor(0, 1);
  lcd.print("LDR:");
  lcd.print(ldrValue);
  lcd.print("%");

  delay(3000);
}
