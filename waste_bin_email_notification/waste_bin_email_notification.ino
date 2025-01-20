#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "OIC";
const char* password = "oichub@@2025";

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
    Serial.readString();

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    String url = "https://hook.eu2.make.com/2irrkxc8wm0mw2c3mtt2721wl4say6nj";

    if (http.begin(client, url)) {
      http.GET();

      http.end();
    }
  }
}
