#define BLYNK_TEMPLATE_ID "TMPL27Y-8WrzF"
#define BLYNK_TEMPLATE_NAME "Green House System"
#define BLYNK_AUTH_TOKEN "5qd-IM2UHNZq-UewGJwIAwMqyv_nBb_-"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESPComm.h>

char ssid[] = "OIC_";
char pass[] = "oichub@@1940";

WiFiClient client;

HardwareSerial mySerial(2);
ESPComm esp(mySerial);

void setup() {
  // put your setup code here, to run once:
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  esp.onCommand(onReceive);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  // put your main code here, to run repeatedly:
  esp.loop();
  Blynk.run();
}

void onReceive(String key, String value) {
  if (key == "DHT1_Temp") {
    Blynk.virtualWrite(V0, value.toFloat());
  } else if (key == "DHT1_Hum") {
    Blynk.virtualWrite(V1, value.toInt());
  } else if (key == "DHT2_Temp") {
    Blynk.virtualWrite(V2, value.toFloat());
  } else if (key == "DHT2_Hum") {
    Blynk.virtualWrite(V3, value.toInt());
  } else if (key == "DS18_1") {
    Blynk.virtualWrite(V4, value.toFloat());
  } else if (key == "DS18_2") {
    Blynk.virtualWrite(V5, value.toFloat());
  } else if (key == "DS18_3") {
    Blynk.virtualWrite(V6, value.toFloat());
  } else if (key == "Pressure") {
    Blynk.virtualWrite(V7, value.toFloat());
  } else if (key == "TDS1") {
    Blynk.virtualWrite(V8, value.toFloat());
  } else if (key == "TDS2") {
    Blynk.virtualWrite(V9, value.toFloat());
  } else if (key == "TDS3") {
    Blynk.virtualWrite(V10, value.toFloat());
  } else if (key == "PH1") {
    Blynk.virtualWrite(V11, value.toFloat());
  } else if (key == "PH2") {
    Blynk.virtualWrite(V12, value.toFloat());
  } else if (key == "PH3") {
    Blynk.virtualWrite(V13, value.toFloat());
  } else if (key == "LDR1") {
    Blynk.virtualWrite(V14, value.toFloat());
  } else if (key == "LDR2") {
    Blynk.virtualWrite(V15, value.toFloat());
  } else if (key == "LDR3") {
    Blynk.virtualWrite(V16, value.toFloat());
  }
}
