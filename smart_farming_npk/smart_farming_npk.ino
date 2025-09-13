#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <ESPComm.h>

ESPComm esp(Serial);

// Define pins
#define DHTPIN 4
#define DHTTYPE DHT11
#define SOIL_PIN A1
#define PUMP 6
#define RE 8
#define DE 7

// Modbus RTU requests for reading NPK values
const byte nitro[] = { 0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c };
const byte phos[] = { 0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc };
const byte pota[] = { 0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0 };

byte values[11];  // store NPK values

SoftwareSerial mod(2, 3);  // RS485 serial
#define NPK_SERIAL mod

LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(DHTPIN, DHTTYPE);

// Timers
unsigned long lastNPKRead = 0;
unsigned long lastDHTRead = 0;
unsigned long lastSoilRead = 0;
unsigned long lastLCDUpdate = 0;
unsigned long lastSend = 0;
unsigned long pumpStartTime = 0;

// Intervals
const unsigned long npkInterval = 2000;   // read NPK every 2s
const unsigned long dhtInterval = 3000;   // read temp/humidity every 3s
const unsigned long soilInterval = 1000;  // read soil every 1s
const unsigned long lcdInterval = 1000;   // update LCD every 1s
const unsigned long sendInterval = 5000;  // send data every 5s

// Pump control
const int soilThreshold = 500;           // adjust for your soil sensor
const unsigned long pumpRunTime = 5000;  // pump ON duration in ms
bool pumpRunning = false;

// Sensor values
byte nitrogenVal = 0;
byte phosphorVal = 0;
byte potassVal = 0;
float temp = 0;
float humd = 0;
int soilValue = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  dht.begin();
  esp.begin(9600);

  NPK_SERIAL.begin(9600);

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  pinMode(PUMP, OUTPUT);
  digitalWrite(PUMP, LOW);  // pump off initially
}

void loop() {
  unsigned long now = millis();

  // --- Read NPK ---
  if (now - lastNPKRead >= npkInterval) {
    nitrogenVal = nitrogen();
    phosphorVal = phosphorous();
    potassVal = potassium();
    lastNPKRead = now;
  }

  // --- Read DHT ---
  if (now - lastDHTRead >= dhtInterval) {
    temp = dht.readTemperature();
    humd = dht.readHumidity();
    lastDHTRead = now;
  }

  // --- Read soil ---
  if (now - lastSoilRead >= soilInterval) {
    soilValue = analogRead(SOIL_PIN);

    // Pump control
    if (soilValue > soilThreshold && !pumpRunning) {
      digitalWrite(PUMP, HIGH);
      pumpRunning = true;
      pumpStartTime = now;
    }

    if (pumpRunning && now - pumpStartTime >= pumpRunTime) {
      digitalWrite(PUMP, LOW);
      pumpRunning = false;
    }

    lastSoilRead = now;
  }

  // --- LCD Update ---
  if (now - lastLCDUpdate >= lcdInterval) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("N:"));
    lcd.print(nitrogenVal);
    lcd.print("mg/kg");

    lcd.setCursor(0, 1);
    lcd.print(F("P:"));
    lcd.print(phosphorVal);
    lcd.print("mg/kg");

    lcd.setCursor(0, 2);
    lcd.print(F("K:"));
    lcd.print(potassVal);
    lcd.print("mg/kg");

    lcd.setCursor(0, 3);
    lcd.print(F("T:"));
    lcd.print(temp);
    lcd.print("C H:");
    lcd.print(humd);
    lcd.print(soilValue < soilThreshold ? " Ok" : " Dry");
    lastLCDUpdate = now;
  }

  // --- Send Data to ESP ---
  if (now - lastSend >= sendInterval) {
    esp.send("nitrogen", nitrogenVal);
    esp.send("phosphor", phosphorVal);
    esp.send("potass", potassVal);
    esp.send("temp", temp);
    esp.send("humd", humd);
    esp.send("soil_value", soilValue);
    lastSend = now;
  }
}

// --- Sensor Functions ---
byte nitrogen() {
  return readNPK(nitro);
}
byte phosphorous() {
  return readNPK(phos);
}
byte potassium() {
  return readNPK(pota);
}

byte readNPK(const byte *cmd) {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delayMicroseconds(100);
  NPK_SERIAL.write(cmd, 8);
  NPK_SERIAL.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  unsigned long t = millis();
  while (NPK_SERIAL.available() < 7 && millis() - t < 200) {
    // wait for response
  }

  for (byte i = 0; i < 7 && NPK_SERIAL.available(); i++) {
    values[i] = NPK_SERIAL.read();
  }

  return values[4];  // main value
}
