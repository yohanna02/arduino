#define BLYNK_TEMPLATE_ID "YourTemplateID"
#define BLYNK_TEMPLATE_NAME "YourTemplateName"
#define BLYNK_AUTH_TOKEN "YourAuthToken"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

void sendCommandToUno(const String &cmd) {
  Serial.println(cmd);
}

// Virtual Pin V0 = Appliance 1
// Virtual Pin V1 = Appliance 2

BLYNK_WRITE(V0) {
  int value = param.asInt();
  if (value) {
    sendCommandToUno("A1ON");
  } else {
    sendCommandToUno("A1OFF");
  }
}

BLYNK_WRITE(V1) {
  int value = param.asInt();
  if (value) {
    sendCommandToUno("A2ON");
  } else {
    sendCommandToUno("A2OFF");
  }
}

void setup() {
  Serial.begin(9600);  // UART communication with Arduino Uno
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();
}

