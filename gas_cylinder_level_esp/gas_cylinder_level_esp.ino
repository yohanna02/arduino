#define BLYNK_TEMPLATE_ID "TMPL2s2JBrduj"
#define BLYNK_TEMPLATE_NAME "Gas Cylinder"
#define BLYNK_AUTH_TOKEN "ZfBq9jVbQxreIeCP9ADtIPLlD_Vs5_8Y"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESPComm.h>

ESPComm esp(Serial);

char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";

void setup() {
  // put your setup code here, to run once:
  esp.begin(9600);
  esp.onCommand(processCommand);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  // put your main code here, to run repeatedly:
  esp.loop();
  Blynk.run();
}

void processCommand(String key, String value) {
  int val = value.toInt();

  if (key == "level") {
    Blynk.virtualWrite(V0, val);
  } else if (key == "gas") {
    Blynk.virtualWrite(V1, val);
  }
}