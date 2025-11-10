#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <ESPComm.h>

ESPComm esp(Serial);

char ssid[] = "Redmi 14C";
char pass[] = "@Runtown22";
WiFiClient client;

unsigned long myChannelNumber = 3060577;
const char* myWriteAPIKey = "TWWVKICN19NQM6XT";

unsigned long sendMillis = 0;

void setup() {
  // put your setup code here, to run once:
  esp.begin(9600);
  esp.onCommand(onReceive);

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(5000);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  esp.loop();

  if (millis() - sendMillis > 20000) {
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    sendMillis = millis();
  }
}

void onReceive(String key, String value) {
  if (key == "Pressure") {
    ThingSpeak.setField(1, value.toFloat());
  } else if (key == "SoilTemp") {
    ThingSpeak.setField(3, value.toFloat());
  } else if (key == "AirTemp") {
    ThingSpeak.setField(4, value.toFloat());
  } else if (key == "SoilMoist") {
    ThingSpeak.setField(5, value.toFloat());
  } else if (key == "Light") {
    ThingSpeak.setField(6, value.toFloat());
  }
}
