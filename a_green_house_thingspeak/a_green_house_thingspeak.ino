#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Keypad.h>
#include <ESPComm.h>

ESPComm esp(Serial);

// ----- LCD -----
LiquidCrystal_I2C lcd(0x27, 20, 4);

// ----- Sensors -----
#define DHTPIN A2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define ONE_WIRE_BUS A3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b(&oneWire);

Adafruit_BMP085 bmp;

// ----- Soil Moisture -----
#define SOIL_PIN A1

// ----- LDR -----
#define LDR_PIN A0

// ----- Actuators -----
#define BULB_PIN 13
#define FAN_INLET 3
#define FAN_OUTLET 4
#define PUMP_PIN 2

// ----- Threshold Defaults -----
float tempThresh = 30.0;
float humThresh = 70.0;
int soilThresh = 500;
int lightThresh = 400;

// ----- Keypad -----
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 5, 6, 7, 8 };
byte colPins[COLS] = { 9, 10, 11, 12 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

unsigned long sendMillis = 0;

// ----- Setup -----
void setup() {
  esp.begin(9600);
  lcd.init();
  lcd.backlight();

  dht.begin();
  ds18b.begin();
  bmp.begin();

  pinMode(BULB_PIN, OUTPUT);
  pinMode(FAN_INLET, OUTPUT);
  pinMode(FAN_OUTLET, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("Greenhouse Init");
  delay(2000);
  lcd.clear();
}

// ----- Loop -----
void loop() {
  // Read Sensors
  float dhtTemp = dht.readTemperature();
  float dhtHum = dht.readHumidity();
  ds18b.requestTemperatures();
  float dsTemp = ds18b.getTempCByIndex(0);
  float pressure = bmp.readPressure() / 100.0;  // hPa

  int soil = analogRead(SOIL_PIN);
  int light = analogRead(LDR_PIN);

  // ----- Control Logic -----
  // Light
  if (light < lightThresh) digitalWrite(BULB_PIN, HIGH);
  else digitalWrite(BULB_PIN, LOW);

  // Fans
  if (dhtTemp > tempThresh) digitalWrite(FAN_OUTLET, HIGH);
  else digitalWrite(FAN_OUTLET, LOW);

  if (dhtHum > humThresh) digitalWrite(FAN_INLET, HIGH);
  else digitalWrite(FAN_INLET, LOW);

  // Pump
  if (soil > soilThresh) digitalWrite(PUMP_PIN, HIGH);
  else digitalWrite(PUMP_PIN, LOW);

  // ----- Display Everything on LCD -----
  lcd.setCursor(0, 0);
  lcd.print("AirT:");
  lcd.print(dhtTemp, 1);
  lcd.print("C H:");
  lcd.print(dhtHum, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("SoilT:");
  lcd.print(dsTemp, 1);
  lcd.print("C SM:");
  lcd.print(soil);

  lcd.setCursor(0, 2);
  lcd.print("Press:");
  lcd.print(pressure, 0);
  lcd.print("hPa L:");
  lcd.print(light);

  lcd.setCursor(0, 3);
  lcd.print("B:");
  lcd.print(digitalRead(BULB_PIN) ? "ON " : "OFF");
  lcd.print(" P:");
  lcd.print(digitalRead(PUMP_PIN) ? "ON " : "OFF");
  lcd.print(" FI:");
  lcd.print(digitalRead(FAN_INLET) ? "ON" : "OFF");
  lcd.print(" FO:");
  lcd.print(digitalRead(FAN_OUTLET) ? "ON" : "OFF");


  if (millis() - sendMillis > 20000) {
    esp.send("AirTemp", String(dhtTemp, 1));
    esp.send("AirHum", String(dhtHum, 0));
    esp.send("SoilTemp", String(dsTemp, 1));
    esp.send("SoilMoist", String(soil));
    esp.send("Pressure", String(pressure, 0));
    esp.send("Light", String(light));

    sendMillis = millis();
  }

  // ----- Keypad (basic demo: press A to raise tempThresh) -----
  char key = keypad.getKey();
  if (key) {
    if (key == 'A') tempThresh += 1;
    if (key == 'B') tempThresh -= 1;
    if (key == 'C') soilThresh += 50;
    if (key == 'D') soilThresh -= 50;
  }

  delay(1000);
}
