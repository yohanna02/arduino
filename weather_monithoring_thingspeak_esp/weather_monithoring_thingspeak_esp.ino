#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

#define mySSID "OIC*"   // WiFi SSID
#define myPWD "oichub@@1940"          // WiFi Password
// #define myAPI "L4EB2EGRQC85UDPW"  // API Key
#define myAPI "7RXUOPYG2QY5YZWU"  // API Key

WiFiClient client;

float dhtHumidity = 0.0;
float dhtTemp = 0.0;
float pressure = 0.0;
float altitude = 0.0;
int soilMoisture = 0;
int rainMoisture = 0;
int lightMoisture = 0;

unsigned long myChannelNumber = 3003490;
const char* myWriteAPIKey = myAPI;

unsigned long prevMillis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(mySSID, myPWD);
      delay(5000);
    }
  }

  ThingSpeak.setField(1, dhtHumidity);
  ThingSpeak.setField(2, dhtTemp);
  ThingSpeak.setField(3, pressure);
  ThingSpeak.setField(4, altitude);
  ThingSpeak.setField(5, soilMoisture);
  ThingSpeak.setField(6, rainMoisture);
  ThingSpeak.setField(7, lightMoisture);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');

    processCommand(command);
  }

  if (millis() - prevMillis > 20000) {
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    prevMillis = millis();
  }
}

void processCommand(String cmd) {
  int sepIndex = cmd.indexOf('=');

  if (sepIndex != -1) {
    String key = cmd.substring(0, sepIndex);
    float value = cmd.substring(sepIndex + 1).toFloat();

    if (key == "humd") {
      ThingSpeak.setField(1, value);
    } else if (key == "temp") {
      ThingSpeak.setField(2, value);
    } else if (key == "pressure") {
      ThingSpeak.setField(3, value);
    } else if (key == "altitude") {
      ThingSpeak.setField(4, value);
    } else if (key == "soil") {
      ThingSpeak.setField(5, value);
    } else if (key == "rain") {
      ThingSpeak.setField(6, value);
    } else if (key == "light") {
      ThingSpeak.setField(7, value);
    }
  }
}
