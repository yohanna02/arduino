#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <LiquidCrystal_I2C.h>

#define ESP_SERIAL Serial
#define seaLevelPressure_hPa 616
#define DHT_PIN 2
#define SOIL_PIN A1
#define RAIN_PIN A2
// #define LDR_PIN A3

#define RELAY 10

Adafruit_BMP085 bmp;
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHT_PIN, DHT11);

int state = 0;
unsigned long displayChange = 0;

unsigned long lastSendTime = 0;
const unsigned long interval = 20000;  // 5 minutes in milliseconds

float dhtHumidity;
float dhtTemp;
float pressure;
float altitude;
int soilMoisture;
int rainMoisture;
// int lightMoisture;

void setup() {
  // put your setup code here, to run once:
  ESP_SERIAL.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(RELAY, OUTPUT);

  if (!bmp.begin()) {
    lcd.clear();
    lcd.print("BMP Error");
    while (1) {}
  }

  lcd.print(F("Integration of"));
  lcd.setCursor(0, 1);
  lcd.print(F("IoT driven smart"));
  delay(3000);
  lcd.clear();

  lcd.print(F("irrigation"));
  lcd.setCursor(0, 1);
  lcd.print(F("system for"));
  delay(3000);
  lcd.clear();

  lcd.print(F("substainable rice"));
  lcd.setCursor(0, 1);
  lcd.print(F("farming in Bayara"));
  delay(3000);
  lcd.clear();

  lcd.print(F("area of bauchi"));
  lcd.setCursor(0, 1);
  lcd.print(F("state nigeria"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Nwenyi.O Blessed"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/19323"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Mallam Hamza"));
  delay(3000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  if (currentMillis - displayChange > 3000) {
    // Read all sensors
    dhtHumidity = dht.readHumidity();
    dhtTemp = dht.readTemperature();
    pressure = bmp.readPressure();
    altitude = bmp.readAltitude();
    soilMoisture = analogRead(SOIL_PIN);
    rainMoisture = analogRead(RAIN_PIN);
    // lightMoisture = analogRead(LDR_PIN);

    soilMoisture = map(soilMoisture, 0, 1023, 100, 0);
    rainMoisture = map(rainMoisture, 0, 1023, 100, 0);
    // lightMoisture = map(lightMoisture, 0, 1023, 100, 0);


    // Display on LCD
    lcd.clear();

    if (state == 0) {
      lcd.print("Temp:" + String(dhtTemp) + "C");
      lcd.setCursor(0, 1);
      lcd.print("Hum:" + String(dhtHumidity) + "%");
    } else if (state == 1) {
      lcd.print("Press:" + String(pressure, 0) + "Pa");
      lcd.setCursor(0, 1);
      lcd.print("Alt:" + String(altitude, 1) + "m");
    } else if (state == 2) {
      lcd.print("Soil Moist:" + String(soilMoisture) + "%");
      lcd.setCursor(0, 1);
      lcd.print("Rain:" + String(rainMoisture) + "%");
    } /* else if (state == 3) {
      lcd.print("Light:" + String(lightMoisture));
    } */

    state++;
    if (state > 2) {
      state = 0;
    }
    displayChange = millis();
  }

  if (soilMoisture < 50) {
    digitalWrite(RELAY, HIGH);
  } else {
    digitalWrite(RELAY, LOW);
  }

  if (currentMillis - lastSendTime > interval) {
    send_data();
    lastSendTime = millis();
  }
}

void sendCommand(String key, float value) {
  ESP_SERIAL.print(key);
  ESP_SERIAL.print("=");
  ESP_SERIAL.print(value, 2);  // send with 2 decimal precision
  ESP_SERIAL.print('\n');
}

void send_data() {
  lcd.clear();
  lcd.print(F("Sending data"));

  sendCommand("humd", dhtHumidity);
  delay(200);

  sendCommand("temp", dhtTemp);
  delay(200);

  sendCommand("pressure", pressure);
  delay(200);

  sendCommand("altitude", altitude);
  delay(200);

  sendCommand("soil", soilMoisture);
  delay(200);

  sendCommand("rain", rainMoisture);
  // delay(200);
  // sendCommand("light", lightMoisture);
  delay(200);
  lcd.clear();
}
