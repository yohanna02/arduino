// API_KEY - KKVMT6XKPL2NRLG8
// CHANNEL_ID - 3066364

#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <ESPComm.h>

char ssid[] = "OIC_";
char pass[] = "oichub@@1940";
WiFiClient client;

unsigned long myChannelNumber = 3066364;
const char* myWriteAPIKey = "KKVMT6XKPL2NRLG8";

ESPComm esp(Serial);

void onReceive(String key, String value) {

}

void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  esp.begin(9600);
  esp.onCommand(onReceive);
}

void loop() {
  // put your main code here, to run repeatedly:
  esp.loop();
}
