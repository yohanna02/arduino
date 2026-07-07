#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NonBlockingDallas.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <Keypad.h>
#include <ESPComm.h>
#include "TDSSensor.h"

// ---------------- SENSORS ----------------
#define DHT1_PIN 2
#define DHT2_PIN 3

#define TDS1_PIN A7
#define TDS2_PIN A6
#define TDS3_PIN A8

#define PH1_PIN A4
#define PH2_PIN A3
#define PH3_PIN A5

#define DS18B1_PIN A10
#define DS18B2_PIN A9
#define DS18B3_PIN A11

#define LDR1_PIN A0
#define LDR2_PIN A1
#define LDR3_PIN A2

// ---------------- RELAYS ----------------
// #define PUMP1_RELAY 27
// #define PUMP2_RELAY 28
// #define PUMP3_RELAY 29
// #define FAN1_RELAY 23
// #define FAN2_RELAY 24
#define UV_RELAY 25
// #define HUMIDIFIER1_RELAY 22
// #define HUMIDIFIER2_RELAY 26
#define FREE1_RELAY 30
#define FREE2_RELAY 31

// === ADC configuration ===
const float ADC_REF = 5.0;
const float ADC_RES = 1023.0;

// === TDS calibration ===
#define TDS_FACTOR 0.5

Adafruit_BMP085 bmp;
LiquidCrystal_I2C lcd(0x27, 20, 4);

DHT dht1(DHT1_PIN, DHT11);
DHT dht2(DHT2_PIN, DHT11);

OneWire oneWire1(DS18B1_PIN);
DallasTemperature dallasTemp1(&oneWire1);
NonBlockingDallas tempSensor1(&dallasTemp1);

OneWire oneWire2(DS18B2_PIN);
DallasTemperature dallasTemp2(&oneWire2);
NonBlockingDallas tempSensor2(&dallasTemp2);

OneWire oneWire3(DS18B3_PIN);
DallasTemperature dallasTemp3(&oneWire3);
NonBlockingDallas tempSensor3(&dallasTemp3);

TDSSensor tds1(TDS1_PIN);
TDSSensor tds2(TDS2_PIN);
TDSSensor tds3(TDS3_PIN);

ESPComm esp(Serial2);

// --- Timers ---
unsigned long lastDHTRead = 0;
unsigned long lastDS18Read = 0;
unsigned long lastBMPRead = 0;
unsigned long lastAnalogRead = 0;
unsigned long lastSendUpdate = 0;
unsigned long pumpOnMillis = 0;
unsigned long pumpOffMillis = 0;

bool pumpOn = true;

// --- Intervals (ms) ---
#define DHT_INTERVAL 5000
#define DS18_INTERVAL 3000
#define BMP_INTERVAL 5000
#define ANALOG_INTERVAL 2000
#define SEND_INTERVAL (60000UL * 30)

#define PUMP_ON_INTERVAL (5UL * 60UL * 1000UL)  // 2 hours
#define PUMP_OFF_INTERVAL (1UL * 60UL * 1000UL)        // 5 minutes

// --- Sensor values ---
float dht1Temp = NAN, dht1Hum = NAN, dht2Temp = NAN, dht2Hum = NAN;
float ds18_1 = NAN, ds18_2 = NAN, ds18_3 = NAN;
float pressure = NAN;
float tds_1 = 0, tds_2 = 0, tds_3 = 0;
float ph1 = 0, ph2 = 0, ph3 = 0;
int ldr1 = 0, ldr2 = 0, ldr3 = 0;

// --- LCD / UI state ---
bool lcdActive = false;
uint8_t activeGroup = 0;  // 0..3 for A,B,C,D groups
bool wifiConnectedIcon = false;

// --- Keypad (4x4) ---
// NOTE: change these pin numbers if they conflict with your hardware.
// Use pins available on your board (Mega/Uno/etc.). This sketch assumes digital pins.
const byte ROWS = 4;
const byte COLS = 4;
byte rowPins[ROWS] = { 11, 10, 9, 8 };  // change if needed
byte colPins[COLS] = { 7, 6, 5, 4 };    // change if needed

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- LDR thresholds (tweak based on your sensors) ---
const int LDR_THRESHOLD1 = 890;  // below -> "dark" (turn relay ON)
const int LDR_THRESHOLD2 = 890;
const int LDR_THRESHOLD3 = 890;

// --- Forward declarations ---
float calculatePH(int pin);
float clampToZero(float val);
void updateLCD();
void drawGroupTemperatures();
void drawGroupHumidity();
void drawGroupWater();
void drawGroupLightRelay();
void handleKeypad();
void applyLdrRelays();
void onReceive(String key, String value);

int onCount = 1;

// ---------------- setup ----------------
void setup() {
  Serial.begin(115200);
  esp.begin(9600);
  esp.onCommand(onReceive);

  tds1.begin();
  tds2.begin();
  tds3.begin();

  // init LCD
  lcd.init();
  lcd.backlight();  // we'll turn off programmatically
  lcd.clear();

  // DHTs and temp sensors
  dht1.begin();
  dht2.begin();
  tempSensor1.begin(NonBlockingDallas::resolution_12, DS18_INTERVAL);
  tempSensor2.begin(NonBlockingDallas::resolution_12, DS18_INTERVAL);
  tempSensor3.begin(NonBlockingDallas::resolution_12, DS18_INTERVAL);

  tempSensor1.onTemperatureChange(handleTemperatureChange1);
  tempSensor2.onTemperatureChange(handleTemperatureChange2);
  tempSensor3.onTemperatureChange(handleTemperatureChange3);

  // BMP
  if (!bmp.begin()) {
    lcd.clear();
    lcd.print("BMP085 Error");
    while (1) { delay(500); }  // fatal error - keep showing
  }

  // Relay pins
  // const int relayPins[] = {
  //   PUMP1_RELAY, PUMP2_RELAY, PUMP3_RELAY,
  //   FAN1_RELAY, FAN2_RELAY, UV_RELAY,
  //   HUMIDIFIER1_RELAY, HUMIDIFIER2_RELAY,
  //   FREE1_RELAY, FREE2_RELAY
  // };
  // for (unsigned i = 0; i < sizeof(relayPins) / sizeof(relayPins[0]); ++i) {
  //   pinMode(relayPins[i], OUTPUT);
  //   digitalWrite(relayPins[i], LOW);  // ensure OFF
  // }
    pinMode(UV_RELAY, OUTPUT);
    digitalWrite(UV_RELAY, LOW);  // ensure OFF

  // LDR pins are analog inputs (no pinMode needed)

  // Start with LCD off
  lcdActive = true;
  lcd.clear();
  lcd.print(F("System Ready"));
  lcd.setCursor(0, 1);
  lcd.print(F("Press A, B, C, D"));

  delay(5000);
  esp.send("PUMP", 1);
}

// ---------------- loop ----------------
void loop() {
  runAutoControl();
  unsigned long now = millis();
  esp.loop();
  tempSensor1.update();
  tempSensor2.update();
  tempSensor3.update();
  tds1.update();
  tds2.update();
  tds3.update();
  handleKeypad();

  // DHT sensors (non-blocking interval)
  if (now - lastDHTRead >= DHT_INTERVAL) {
    lastDHTRead = now;
    float t1 = dht1.readTemperature();
    float h1 = dht1.readHumidity();
    float t2 = dht2.readTemperature();
    float h2 = dht2.readHumidity();
    if (!isnan(t1)) dht1Temp = t1;
    if (!isnan(h1)) dht1Hum = h1;
    if (!isnan(t2)) dht2Temp = t2;
    if (!isnan(h2)) dht2Hum = h2;
  }

  // BMP
  if (now - lastBMPRead >= BMP_INTERVAL) {
    lastBMPRead = now;
    pressure = bmp.readPressure();
  }

  // Analog sensors (TDS, pH, LDR)
  if (now - lastAnalogRead >= ANALOG_INTERVAL) {
    lastAnalogRead = now;

    tds_1 = tds1.getTDS(ds18_1);
    tds_2 = tds2.getTDS(ds18_2);
    tds_3 = tds3.getTDS(ds18_3);

    ph1 = calculatePH(PH1_PIN) + 6.5;
    ph2 = calculatePH(PH2_PIN) + 6.5;
    ph3 = calculatePH(PH3_PIN) + 6.5;

    ldr1 = analogRead(LDR1_PIN);
    ldr2 = analogRead(LDR2_PIN);
    ldr3 = analogRead(LDR3_PIN);

    applyLdrRelays();  // check ldrs and set relays
  }

  // Send telemetry periodically
  if (now - lastSendUpdate >= SEND_INTERVAL) {
    lastSendUpdate = now;
    esp.send("DHT1_Temp", dht1Temp);
    esp.send("DHT1_Hum", dht1Hum);
    esp.send("DHT2_Temp", dht2Temp);
    esp.send("DHT2_Hum", dht2Hum);

    esp.send("DS18_1", ds18_1);
    esp.send("DS18_2", ds18_2);
    esp.send("DS18_3", ds18_3);

    esp.send("Pressure", pressure);

    esp.send("TDS1", tds_1);
    esp.send("TDS2", tds_2);
    esp.send("TDS3", tds_3);

    esp.send("PH1", ph1);
    esp.send("PH2", ph2);
    esp.send("PH3", ph3);
  }

  // if (now - pumpOffMillis > PUMP_OFF_INTERVAL && pumpOn) {
  //   pumpOn = false;
  //   pumpOnMillis = millis();
  //   // esp.send("PUMP", 0);
  //   // pumpOffMillis = millis();
  // } else if (now - pumpOnMillis > PUMP_ON_INTERVAL && !pumpOn) {
  //   pumpOn = true;
  //   pumpOffMillis = millis();
  //   // esp.send("PUMP", 1);
  // }

  // if (pumpOn) {
  //   digitalWrite(PUMP1_RELAY, HIGH);
  //   digitalWrite(PUMP2_RELAY, HIGH);
  //   digitalWrite(PUMP3_RELAY, HIGH);
  // } else {
  //   digitalWrite(PUMP1_RELAY, LOW);
  //   digitalWrite(PUMP2_RELAY, LOW);
  //   digitalWrite(PUMP3_RELAY, LOW);
  // }

  // (other non-blocking tasks can go here)
}

void runAutoControl() {

  // ----------- AUTO TEMP CONTROL (Fans) -----------


  // static bool tempState1 = false;
  // static bool tempState2 = false;

  // if (!tempState1 && dht1Temp > 25.0) {
  //   digitalWrite(FAN1_RELAY, HIGH);
  //   tempState1 = true;
  // } else if (tempState1 && dht1Temp < 20.0) {
  //   digitalWrite(FAN1_RELAY, LOW);
  //   tempState1 = false;
  // }

  // if (!tempState2 && dht2Temp > 28.0) {
  //   tempState2 = true;
  //   digitalWrite(FAN2_RELAY, HIGH);
  // } else if (tempState2 && dht2Temp < 20.0) {
  //   digitalWrite(FAN2_RELAY, LOW);
  //   tempState2 = false;
  // }


  // ----------- AUTO HUMIDITY CONTROL -----------
  // static bool humidifierState = false;
  // float avgHum = (dht1Hum + dht2Hum) / 2.0;

  // if (!humidifierState && avgHum < 60) {
  //   digitalWrite(HUMIDIFIER1_RELAY, HIGH);
  //   digitalWrite(HUMIDIFIER2_RELAY, HIGH);
  //   humidifierState = true;
  // }
  // if (humidifierState && avgHum > 75) {
  //   digitalWrite(HUMIDIFIER1_RELAY, LOW);
  //   digitalWrite(HUMIDIFIER2_RELAY, LOW);
  //   humidifierState = false;
  // }


  // ----------- AUTO WATER QUALITY CONTROL -----------
  // Example: pump clean water when TDS is too high
  // TDS > 600 => flush
  // TDS < 500 => stop flush
  // static bool flushPumpState = false;

  // if (!flushPumpState && tds1 > 600) {
  //   digitalWrite(PUMP1_RELAY, HIGH);   // flush pump ON
  //   flushPumpState = true;
  // }
  // if (flushPumpState && tds1 < 500) {
  //   digitalWrite(PUMP1_RELAY, LOW);    // flush pump OFF
  //   flushPumpState = false;
  // }

  // Example: nutrient pump when TDS too low
  // static bool nutrientPumpState = false;

  // if (!nutrientPumpState && tds1 < 300) {
  //   digitalWrite(PUMP2_RELAY, HIGH);   // nutrient ON
  //   nutrientPumpState = true;
  // }
  // if (nutrientPumpState && tds1 > 350) {
  //   digitalWrite(PUMP2_RELAY, LOW);
  //   nutrientPumpState = false;
  // }


  // ----------- AUTO pH CONTROL -----------
  // Example: acid pump when pH > 6.8
  // Example: base pump when pH < 5.8
  // static bool acidState = false;
  // static bool baseState = false;

  // if (!acidState && ph1 > 6.8) {
  //   digitalWrite(PUMP3_RELAY, HIGH);  // acid
  //   acidState = true;
  // }
  // if (acidState && ph1 < 6.5) {
  //   digitalWrite(PUMP3_RELAY, LOW);
  //   acidState = false;
  // }
}


void handleTemperatureChange1(int device, int32_t temperatureRAW) {
  ds18_1 = tempSensor1.rawToCelsius(temperatureRAW);
}

void handleTemperatureChange2(int device, int32_t temperatureRAW) {
  ds18_2 = tempSensor2.rawToCelsius(temperatureRAW);
}

void handleTemperatureChange3(int device, int32_t temperatureRAW) {
  ds18_3 = tempSensor3.rawToCelsius(temperatureRAW);
}

// ---------------- Keypad handling ----------------
void handleKeypad() {
  char k = keypad.getKey();
  if (k) {
    lcd.clear();

    if (k == 'A' || k == 'B' || k == 'C' || k == 'D') {
      if (k == 'A') activeGroup = 0;
      if (k == 'B') activeGroup = 1;
      if (k == 'C') activeGroup = 2;
      if (k == 'D') activeGroup = 3;
      updateLCD();  // immediate update when group selected
    } else {
      // optional: other keys can toggle pumps / manual actions
      // e.g., '*' toggles pump1 for manual override (not implemented automatic)
    }
  }
}

// ---------------- LDR -> Relay logic ----------------
void applyLdrRelays() {
  // Each LDR toggles a particular relay when it is "dark" (value < threshold)
  if (ldr1 < LDR_THRESHOLD1 || ldr2 < LDR_THRESHOLD2 || ldr3 < LDR_THRESHOLD3) {
    if (digitalRead(UV_RELAY) == LOW) {
      esp.send("LIGHT", 1);
    }
    digitalWrite(UV_RELAY, HIGH);
  } else {
    if (digitalRead(UV_RELAY) == HIGH) {
      esp.send("LIGHT", 0);
    }
    digitalWrite(UV_RELAY, LOW);
  }

  // if (ldr2 < LDR_THRESHOLD2) digitalWrite(FREE1_RELAY, HIGH);
  // else digitalWrite(FREE1_RELAY, LOW);

  // if (ldr3 < LDR_THRESHOLD3) digitalWrite(FREE2_RELAY, HIGH);
  // else digitalWrite(FREE2_RELAY, LOW);
}

// ---------------- LCD drawing & groups ----------------
void updateLCD() {
  if (!lcdActive) return;  // nothing to do

  lcd.clear();
  switch (activeGroup) {
    case 0: drawGroupTemperatures(); break;
    case 1: drawGroupHumidity(); break;
    case 2: drawGroupWater(); break;
    case 3: drawGroupLightRelay(); break;
    default: drawGroupTemperatures(); break;
  }
}

// Group A: temperatures
void drawGroupTemperatures() {
  // Row 0: DHT temps with thermometer icon
  lcd.setCursor(0, 0);
  lcd.print("T1:");
  if (!isnan(dht1Temp)) {
    lcd.print(dht1Temp, 1);
  } else {
    lcd.print("ERR");
  }
  lcd.print((char)223);  // degree symbol
  lcd.print("C");

  lcd.print(" T2:");
  if (!isnan(dht2Temp)) lcd.print(dht2Temp, 1);
  else lcd.print("ERR");
  lcd.print((char)223);
  lcd.print("C");

  // Row 2: DS18B20s (show 2 per row)
  lcd.setCursor(0, 1);
  lcd.print("DS1:");
  if (!isnan(ds18_1)) lcd.print(ds18_1, 1);
  else lcd.print("ERR");
  lcd.print((char)223);
  lcd.print("C");

  lcd.print(" DS2:");
  if (!isnan(ds18_2)) lcd.print(ds18_2, 1);
  else lcd.print("ERR");
  lcd.print((char)223);
  lcd.print("C");

  // Row 3: DS3 and pressure
  lcd.setCursor(0, 2);
  lcd.print("DS3:");
  if (!isnan(ds18_3)) lcd.print(ds18_3, 1);
  else lcd.print("ERR");
  lcd.print((char)223);
  lcd.print("C");

  lcd.print(" P:");
  if (!isnan(pressure)) {
    float hpa = pressure / 100.0;
    lcd.print(hpa, 0);
    lcd.print("hPa");
  } else lcd.print("ERR");


  // lcd.setCursor(0, 3);
  // // show fan relay states
  // lcd.print("F1:");
  // lcd.print(digitalRead(FAN1_RELAY) ? "On " : "Off ");
  // lcd.print(" F2:");
  // lcd.print(digitalRead(FAN1_RELAY) ? "On " : "Off ");
}

// Group B: humidity
void drawGroupHumidity() {
  lcd.setCursor(0, 0);
  lcd.print(" DHT1 H:");
  if (!isnan(dht1Hum)) lcd.print(dht1Hum, 1);
  else lcd.print("ERR");
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print(" DHT2 H:");
  if (!isnan(dht2Hum)) lcd.print(dht2Hum, 1);
  else lcd.print("ERR");
  lcd.print("%");

  // lcd.setCursor(0, 2);
  // lcd.print("Humidifiers:");
  // lcd.setCursor(0, 3);
  // // show humidifier relay states
  // lcd.print("H1:");
  // lcd.print(digitalRead(HUMIDIFIER1_RELAY) ? "On " : "Off ");
  // lcd.print(" H2:");
  // lcd.print(digitalRead(HUMIDIFIER2_RELAY) ? "On " : "Off ");
}

// Group C: TDS & pH
void drawGroupWater() {
  lcd.setCursor(0, 0);
  lcd.print("TDS1:");
  lcd.print(tds_1, 0);

  lcd.print("   TDS2:");
  lcd.print(tds_2, 0);

  lcd.setCursor(0, 1);
  lcd.print("TDS3:");
  lcd.print(tds_3, 0);
  lcd.print("   PH1:");
  lcd.print(ph1, 1);

  lcd.setCursor(0, 3);
  lcd.print("PH2:");
  lcd.print(ph2, 1);
  lcd.print("    PH3:");
  lcd.print(ph3, 1);
}

// Group D: LDR / relays / WiFi
void drawGroupLightRelay() {
  // animated sun icon for light
  lcd.setCursor(0, 0);
  lcd.print(" LDR1:");
  lcd.print(ldr1);
  lcd.print(" R:");
  lcd.print(digitalRead(UV_RELAY) ? "On " : "Off ");

  lcd.setCursor(0, 1);
  lcd.print(" LDR2:");
  lcd.print(ldr2);
  lcd.print(" R:");
  lcd.print(digitalRead(FREE1_RELAY) ? "On " : "Off ");

  lcd.setCursor(0, 2);
  lcd.print(" LDR3:");
  lcd.print(ldr3);
  lcd.print(" R:");
  lcd.print(digitalRead(FREE2_RELAY) ? "On " : "Off ");

  // WiFi icon + label
  lcd.setCursor(0, 3);
  if (wifiConnectedIcon) {
    lcd.print(" WiFi: Connected ");
  } else {
    lcd.print(" WiFi: Discounted");
  }
}

// ----------------- Helpers -----------------
float calculatePH(int pin) {
  const int samples = 10;
  int buffer_arr[samples];
  for (int i = 0; i < samples; i++) {
    buffer_arr[i] = analogRead(pin);
    delay(5);  // very short settling between reads
  }

  // sort (simple insertion sort would be slightly faster, but this is fine)
  for (int i = 0; i < samples - 1; i++) {
    for (int j = i + 1; j < samples; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        int t = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = t;
      }
    }
  }

  unsigned long avgval = 0;
  for (int i = 2; i < samples - 2; i++) avgval += buffer_arr[i];
  float voltage = (float)avgval * ADC_REF / ADC_RES / (samples - 4);

  // Calibration constants — tune these with calibration solutions
  float calibration_value = 21.34 - 0.7;
  float ph = -5.70 * voltage + calibration_value;
  return constrain(ph, 0.0, 14.0);
}

float clampToZero(float val) {
  return (val < 0.0f) ? 0.0f : val;
}

// -------------- ESPComm callback --------------
void onReceive(String key, String value) {
  if (key == "WIFI") {
    if (value == "CONNECT") {
      wifiConnectedIcon = true;
      updateLCD();
    } else if (value == "DISCONNECT") {
      wifiConnectedIcon = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi Disconnected");
      // small delay just to let the user see message (short, acceptable)
      delay(800);
      updateLCD();
    }
  }
}
