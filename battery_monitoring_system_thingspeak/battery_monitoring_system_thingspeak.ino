#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <ESPComm.h>

// char ssid[] = "Galaxy A14";  // your network SSID (name)
// char pass[] = "Marthana04";  // your network password
char ssid[] = "Becky";  // your network SSID (name)
char pass[] = "12331234";  // your network password
WiFiClient client;

unsigned long myChannelNumber = 3162772;
const char* myWriteAPIKey = "05KBPDG721Q1QH9N";

ESPComm esp(Serial);

unsigned long prevMillis = 0;

void setup() {
  // put your setup code here, to run once:
  esp.begin(9600);
  esp.onCommand(onReceive);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
    }
  }
  prevMillis = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  esp.loop();

  if (millis() - prevMillis > 20000) {
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    prevMillis = millis();
  }
}

void onReceive(String key, String value) {
  if (key == "s1") {
    ThingSpeak.setField(1, value.toFloat());
  }
  else if (key == "s2") {
    ThingSpeak.setField(2, value.toFloat());
  }
  else if (key == "s3") {
    ThingSpeak.setField(3, value.toFloat());
  }
  else if (key == "s4") {
    ThingSpeak.setField(4, value.toFloat());
  } else if (key == "temp") {
    ThingSpeak.setField(5, value.toFloat());
  } else if (key == "humd") {
    ThingSpeak.setField(6, value.toFloat());
  }
}
