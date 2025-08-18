#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

// LCD address (often 0x27 or 0x3F for 16x2 I2C LCD)
LiquidCrystal lcd(A5, A4, A3, A2, A1, 13);

// GPS setup
static const int RXPin = 2, TXPin = 3;

TinyGPSPlus gps;
SoftwareSerial _serial(RXPin, TXPin);

#define ESP_SERIAL Serial
#define gpsSerial _serial

unsigned long previous_millis = 0;

void setup() {
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("GPS Location");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  ESP_SERIAL.begin(9600);
  gpsSerial.begin(9600);
  delay(2000);
  lcd.clear();
}

void loop() {
  // Feed GPS with incoming data
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isValid() && millis() - previous_millis > 30000) {
    double lat = gps.location.lat();
    double lon = gps.location.lng();

    sendCommand("lon", lon);
    delay(200);
    sendCommand("lat", lat);
    previous_millis = millis();
  }

  if (gps.location.isUpdated() && gps.location.isValid()) {
    double lat = gps.location.lat();
    double lon = gps.location.lng();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lat: ");
    lcd.print(lat, 6);  // 6 decimal places

    lcd.setCursor(0, 1);
    lcd.print("Lon: ");
    lcd.print(lon, 6);
  } else if (!gps.location.isValid()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Getting");
    lcd.setCursor(0, 1);
    lcd.print("Location...");
  }

  delay(1000);
}

void sendCommand(String key, double value) {
  ESP_SERIAL.print(key);
  ESP_SERIAL.print("=");
  ESP_SERIAL.print(value, 6);  // send with 2 decimal precision
  ESP_SERIAL.print('\n');
}
