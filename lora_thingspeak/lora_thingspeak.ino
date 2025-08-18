#include <WiFi.h>
#include <ThingSpeak.h>

char ssid[] = "NE_2.4G_345D3F";
char pass[] = "D392B417";

WiFiClient client;

unsigned long myChannelNumber = 3003490;
const char* myWriteAPIKey = "ZCNYBYOE8I7Q78GY";

int AQI = 0;
int TVOC = 0;
int eCO2 = 0;
byte N = 0;
float CH4 = 0;

unsigned long prevMillis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      delay(5000);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String incoming = "";
    while (Serial.available()) {
      char value = Serial.read();
      if (value == ',') {
        processCommand(incoming);
        incoming = "";
        continue;
      }
      incoming += value;

    }
    processCommand(incoming);
  }

  if (millis() - prevMillis > 20000) {
    ThingSpeak.setField(1, CH4);
    ThingSpeak.setField(2, N);
    ThingSpeak.setField(3, TVOC);
    ThingSpeak.setField(4, eCO2);
    ThingSpeak.setField(5, AQI);

    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    prevMillis = millis();
  }
}


void processCommand(String cmd) {
  int sepIndex = cmd.indexOf('=');

  if (sepIndex != -1) {
    String key = cmd.substring(0, sepIndex);
    float value = cmd.substring(sepIndex + 1).toFloat();

    if (key == "CH4") {
      CH4 = value;
    } else if (key == "N") {
      N = value;
    } else if (key == "TVOC") {
      TVOC = value;
    } else if (key == "eCO2") {
      eCO2 = value;
    } else if (key == "AQI") {
      AQI = value;
    }
  }
}
