#include <WiFi.h>
#include "ThingSpeak.h"
#include "DHT.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ====== CONFIGURATION ======
#define DHTPIN 32
#define DHTTYPE DHT11  // or DHT11
#define SOIL_PIN 33    // Analog pin for soil sensor
#define RELAY_PIN 23
#define SOIL_THRESHOLD 25  // Percent: below this means dry soil

// WiFi credentials
const char* ssid = "OIC_";
const char* password = "oichub@@1940";

// ThingSpeak details
unsigned long myChannelNumber = 3161173;
const char* myWriteAPIKey = "S6RLTWFCBGLXAHHY";

WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);

unsigned long previousMillis = 0;
unsigned long readPreviousMillis = 0;
const long interval = 20000;  // Send every 20s

float humidity = 0.0;
float temperature = 0.0;
int soilPercent = 0;
int pumpStatus = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  lcd.setCursor(0, 0);
  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("implementation"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("of a plant health"));
  lcd.setCursor(0, 1);
  lcd.print(F("monitoring system"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Stephen Matthew ogbe"));
  lcd.setCursor(0, 1);
  lcd.print(F("BPU/EEE/H/EEP/23/10027"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Odoh Gabriel"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr.justin kpelai.A"));
  delay(3000);
  lcd.clear();

  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print(F("WiFi"));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  lcd.clear();
  lcd.print("WiFi connected!");
  delay(3000);
  lcd.clear();
}

void loop() {
  // Ensure WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting WiFi...");
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("WiFi failed!");
      return;
    }
  }

  unsigned long currentMillis = millis();

  if (currentMillis - readPreviousMillis > 2000) {
    // Read DHT sensor
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    // Read soil moisture (convert analog value to percentage)
    int soilValue = analogRead(SOIL_PIN);
    // For most sensors: 0 = wet, 4095 = dry (ESP32 ADC is 12-bit)
    soilPercent = map(soilValue, 4092, 0, 0, 100);
    soilPercent = constrain(soilPercent, 0, 100);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temperature, 1);
    lcd.print("C H:");
    lcd.print(humidity, 0);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("S:");
    lcd.print(soilPercent);
    lcd.print("% ");

    if (pumpStatus == 1) lcd.print("Pump:ON");
    else lcd.print("Pump:OFF");

    readPreviousMillis = millis();
  }

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // Send data to ThingSpeak
    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, soilPercent);
    ThingSpeak.setField(4, pumpStatus);

    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  }

  if (soilPercent < SOIL_THRESHOLD) {
    digitalWrite(RELAY_PIN, HIGH);  // Turn ON pump
    pumpStatus = 1;
  } else {
    digitalWrite(RELAY_PIN, LOW);  // Turn OFF pump
    pumpStatus = 0;
  }
}
