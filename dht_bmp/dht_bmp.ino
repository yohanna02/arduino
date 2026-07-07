#include <Wire.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>

// LCD
LiquidCrystal lcd(A0, A1, A2, A3, 12, 11);

// DHT11
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// BMP180
Adafruit_BMP085 bmp;

unsigned long previousMillis = 0;
const long interval = 3000;
byte screen = 0;

void setup()
{
  lcd.begin(16,2);

  dht.begin();

  if (!bmp.begin())
  {
    lcd.print("BMP180 Error");
    while (1);
  }

  lcd.setCursor(0, 0);
  lcd.print("Ambient parameters");
  lcd.setCursor(0, 1);
  lcd.print("monitoring device");
  delay(2000);
  lcd.clear();
}

void loop()
{
  float humidity = dht.readHumidity();
  float dhtTemp = dht.readTemperature();

  float bmpTemp = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0; // hPa
  float altitude = bmp.readAltitude();

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    screen++;

    if(screen > 2)
      screen = 0;

    lcd.clear();
  }

  switch(screen)
  {
    case 0:

      lcd.setCursor(0,0);
      lcd.print("Temp:");
      lcd.print(dhtTemp,1);
      lcd.print((char)223);
      lcd.print("C");

      lcd.setCursor(0,1);
      lcd.print("Hum:");
      lcd.print(humidity,0);
      lcd.print("%");

      break;

    case 1:

      lcd.setCursor(0,0);
      lcd.print("Pressure:");

      lcd.setCursor(0,1);
      lcd.print(pressure,0);
      lcd.print(" hPa");

      break;

    case 2:

      lcd.setCursor(0,0);
      lcd.print("Altitude:");

      lcd.setCursor(0,1);
      lcd.print(altitude,1);
      lcd.print(" m");

      break;
  }

  delay(500);
}