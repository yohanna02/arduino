#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "OIC";
const char* password = "oichub@2026";

struct Voter {
  String cardId;
  String name;
};

#define NUM_OF_VOTERS 3

Voter voters[NUM_OF_VOTERS] = {
  Voter{ .cardId = "B3267A06", .name = "John" },
  Voter{ .cardId = "938F8D03", .name = "Musa" },
  Voter{ .cardId = "D06EFF58", .name = "Ibrahim" }
};

// https://hook.eu2.make.com/o4j93iyxi5qd7vbsl4p1v07hxgfda6nb?name=yohanna&party=B&id=SDS3jj2A&type=clear

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String type = Serial.readStringUntil(':');
    int idx = Serial.readStringUntil(':').toInt();
    String party = Serial.readString();

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    String url = "https://hook.eu2.make.com/o4j93iyxi5qd7vbsl4p1v07hxgfda6nb?type=" + type;

    if (type == "vote") {
      url += "&name=" + voters[idx].name;
      url += "&id=" + voters[idx].cardId;
      url += "&party=" + party;
    }

    if (http.begin(client, url)) {
      int httpCode = http.GET();

      if (httpCode == HTTP_CODE_OK) {
        Serial.print(http.getString());
      } else {
        Serial.print(F("ERROR"));
      }
    }
  }
}
