#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

#define mySSID "OIC*"             // WiFi SSID
#define myPWD "oichub@@1940"      // WiFi Password
#define myAPI "TWOO3JJIC7YAOR5D"  // API Key

WiFiClient client;

double lat = 0.0;
double lon = 0.0;

unsigned long myChannelNumber = 1978416;
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
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');

    processCommand(command);
  }

  if (millis() - prevMillis > 30000 && lat != 0.0 && lon != 0.0) {
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    prevMillis = millis();
  }
}

void processCommand(String cmd) {
  int sepIndex = cmd.indexOf('=');

  if (sepIndex != -1) {
    String key = cmd.substring(0, sepIndex);
    double value = cmd.substring(sepIndex + 1).toDouble();

    if (key == "lat") {
      lat = value;
      ThingSpeak.setField(1, static_cast<float>(value));
    } else if (key == "lon") {
      lon = value;
      ThingSpeak.setField(2, static_cast<float>(value));
    }
  }
}
