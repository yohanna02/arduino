#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <ESPComm.h>

ESPComm esp(Serial);
WiFiClient client;

char ssid[] = "OIC_";
char pass[] = "oichub@@1940";

unsigned long myChannelNumber = 2602236;
const char* myWriteAPIKey = "7LCX6WZUVPJ82C4I";

unsigned long prevMillis = 0;

void setup() {
  // put your setup code here, to run once:
  esp.begin(9600);
  esp.onCommand(processCommand);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      delay(5000);
    }
  }

  ThingSpeak.setField(1, 0);
  ThingSpeak.setField(2, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  esp.loop();

  if (millis() - prevMillis > 20000) {
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    prevMillis = millis();
  }
}

void processCommand(String key, String value) {
  int val = value.toInt();

  if (key == "PERCENT") {
    ThingSpeak.setField(1, val);
  } else if (key == "GAS_DETECTED") {
    ThingSpeak.setField(2, val);
  }
}