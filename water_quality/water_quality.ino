#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>

// === Pin definitions ===
#define TEMP_PIN 23
#define PH_PIN 32
#define TDS_PIN 35
#define TURBIDITY_PIN 26
#define BUZZER_PIN 15  // Buzzer on GPIO 15

// === LCD setup ===
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === WiFi credentials ===
const char* ssid = "Monbebe";
const char* password = "11111112";

// === ThingSpeak settings ===
const char* server = "api.thingspeak.com";
const String apiKey = "TX6XKSR75KXYG4NO";

// === DS18B20 Temperature sensor setup ===
OneWire oneWire(TEMP_PIN);
DallasTemperature tempSensor(&oneWire);

// === ADC configuration ===
const float ADC_REF = 3.3;
const int ADC_RES = 4095;
const int NUM_SAMPLES = 20;

// === TDS & Turbidity calibration ===
#define TDS_FACTOR 0.5
#define TURBIDITY_OFFSET 0

// === Data variables ===
float temperature = 25.0;
float pHValue = 0.0;
float tdsValue = 0.0;
float turbidityNTU = 0.0;
String status = "Unknown";

// === Timing variables ===
unsigned long lastUploadTime = 0;
const unsigned long uploadInterval = 15000; // 15 seconds
unsigned long lastBeepTime = 0;
const unsigned long beepInterval = 5000;

// === Function Prototypes ===
float calculatePH(int pin);
float readAvgAnalog(int pin);
float clampToZero(float val);
void connectToWiFi();
void sendToThingSpeak();
String evaluateWaterQuality();

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  tempSensor.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  lcd.setCursor(0, 0);
  lcd.print("Water Quality");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  
  connectToWiFi();
  lcd.clear();
}

void loop() {
  // === Temperature ===
  tempSensor.requestTemperatures();
  temperature = tempSensor.getTempCByIndex(0);
  if (temperature == -127 || temperature == 85) temperature = 25.0;

  // === pH ===
  pHValue = calculatePH(PH_PIN);

  // === TDS ===
  float tdsRaw = readAvgAnalog(TDS_PIN);
  float tdsVoltage = tdsRaw * ADC_REF / ADC_RES;
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  float compensatedVoltage = tdsVoltage / compensationCoefficient;
  float ec = (133.42 * pow(compensatedVoltage, 3)) - (255.86 * pow(compensatedVoltage, 2)) + (857.39 * compensatedVoltage);
  tdsValue = clampToZero(ec * TDS_FACTOR);

  // === Turbidity ===
  float turbRaw = readAvgAnalog(TURBIDITY_PIN);
  float turbVoltage = turbRaw * ADC_REF / ADC_RES;
  turbidityNTU = -1120.4 * pow(turbVoltage, 2) + 5742.3 * turbVoltage - 4352.9 + TURBIDITY_OFFSET;
  turbidityNTU = clampToZero(turbidityNTU);

  // === Evaluate water quality ===
  status = evaluateWaterQuality();

  // === Serial Output ===
  Serial.println("------- SENSOR READINGS -------");
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" °C");
  Serial.print("pH: "); Serial.println(pHValue);
  Serial.print("TDS: "); Serial.print(tdsValue); Serial.println(" ppm");
  Serial.print("Turbidity: "); Serial.print(turbidityNTU); Serial.println(" NTU");
  Serial.print("Status: "); Serial.println(status);

  // === LCD Display ===
  lcd.setCursor(0, 0);
  lcd.print("T:" + String(temperature,1) + "C pH:" + String(pHValue,1));
  lcd.setCursor(0, 1);
  lcd.print("TDS:" + String((int)tdsValue) + " NTU:" + String((int)turbidityNTU) + " ");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("Status: " + status);

  // === Send to ThingSpeak ===
  if (millis() - lastUploadTime >= uploadInterval) {
    sendToThingSpeak();
    lastUploadTime = millis();
  }

  delay(1000);
}

// === Function Definitions ===

float calculatePH(int pin) {
  int buffer_arr[10];
  unsigned long avgval = 0;
  int temp;

  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(pin);
    delay(30);
  }

  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }

  for (int i = 2; i < 8; i++) avgval += buffer_arr[i];

  float voltage = (float)avgval * 3.3 / 4095 / 6;
  float calibration_value = 21.34 - 0.7;
  float ph = -5.70 * voltage + calibration_value;

  return constrain(ph, 0.0, 14.0);
}

float readAvgAnalog(int pin) {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / (float)NUM_SAMPLES;
}

float clampToZero(float val) {
  return (val < 0.0f) ? 0.0f : val;
}

void connectToWiFi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("WiFi...");
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");
    delay(2000);
  } else {
    Serial.println("\nFailed to connect to WiFi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed");
    delay(2000);
  }
}

void sendToThingSpeak() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Attempting to reconnect...");
    connectToWiFi();
    if (WiFi.status() != WL_CONNECTED) return;
  }
  
  HTTPClient http;
  
  String url = "http://" + String(server) + "/update?api_key=" + apiKey + 
               "&field1=" + String(pHValue) + 
               "&field2=" + String(temperature) + 
               "&field3=" + String(tdsValue) + 
               "&field4=" + String(turbidityNTU) + 
               "&field5=" + String(status);
  
  Serial.println("Sending data to ThingSpeak: " + url);
  
  http.begin(url);
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

String evaluateWaterQuality() {
  // Weighted scoring
  float score = 0;

  // pH weight 30
  if (pHValue >= 6.5 && pHValue <= 8.5) score += 30;
  else if (pHValue >= 6 && pHValue <= 9) score += 15;

  // TDS weight 35
  if (tdsValue < 500) score += 35;
  else if (tdsValue < 700) score += 15;

  // Turbidity weight 35
  if (turbidityNTU < 5) score += 35;
  else if (turbidityNTU < 10) score += 15;

  // Determine water status
  String level;
  if (score >= 85) {
    level = "Excellent";
    digitalWrite(BUZZER_PIN, LOW);
  } else if (score >= 70) {
    level = "Good";
    if (millis() - lastBeepTime >= beepInterval) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(150);
      digitalWrite(BUZZER_PIN, LOW);
      lastBeepTime = millis();
    }
  } else if (score >= 50) {
    level = "Fair";
    if (millis() - lastBeepTime >= beepInterval) {
      for (int i = 0; i < 2; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(200);
        digitalWrite(BUZZER_PIN, LOW);
        delay(200);
      }
      lastBeepTime = millis();
    }
  } else {
    level = "Poor";
    if (millis() - lastBeepTime >= beepInterval) {
      for (int i = 0; i < 3; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(300);
        digitalWrite(BUZZER_PIN, LOW);
        delay(200);
      }
      lastBeepTime = millis();
    }
  }

  return level;
}
