#define BLYNK_TEMPLATE_ID "TMPL27Y-8WrzF"
#define BLYNK_TEMPLATE_NAME "Green House System"
#define BLYNK_AUTH_TOKEN "5qd-IM2UHNZq-UewGJwIAwMqyv_nBb_-"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESPComm.h>

char ssid[] = "Janga";
char pass[] = "jangammadaki@";

// WiFi name: Janga 
// Password: jangammadaki@

WiFiClient client;

HardwareSerial mySerial(2);
ESPComm esp(mySerial);

#define SEND_INTERVAL (60000UL * 30)
BlynkTimer timer;

// --- Sensor values ---
float dht1Temp = NAN, dht1Hum = NAN, dht2Temp = NAN, dht2Hum = NAN;
float ds18_1 = NAN, ds18_2 = NAN, ds18_3 = NAN;
float pressure = NAN;
float tds1 = 0, tds2 = 0, tds3 = 0;
float ph1 = 0, ph2 = 0, ph3 = 0;
int ldr1 = 0, ldr2 = 0, ldr3 = 0;

void setup() {
  // put your setup code here, to run once:
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  esp.onCommand(onReceive);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(SEND_INTERVAL, sendAll);
}

void loop() {
  // put your main code here, to run repeatedly:
  esp.loop();
  Blynk.run();
  timer.run();
}

BLYNK_CONNECTED() {
  esp.send("WIFI", "CONNECT");
}

BLYNK_DISCONNECTED() {
  esp.send("WIFI", "DISCONNECT");
}

void sendAll() {
  sendDHT();
  timer.setTimeout(60000UL, sendDS18);
  timer.setTimeout(60000UL * 2, sendTDS);
  timer.setTimeout(60000UL * 3, sendPH);
}

void sendDHT() {
  Blynk.virtualWrite(V0, dht1Temp);
  Blynk.virtualWrite(V1, dht1Hum);
  Blynk.virtualWrite(V2, dht2Temp);
  Blynk.virtualWrite(V3, dht2Hum);
}

void sendDS18() {
  Blynk.virtualWrite(V4, ds18_1);
  Blynk.virtualWrite(V5, ds18_2);
  Blynk.virtualWrite(V6, ds18_3);
}

void sendTDS() {
  Blynk.virtualWrite(V8, tds1);
  Blynk.virtualWrite(V9, tds2);
  Blynk.virtualWrite(V10, tds3);
}

void sendPH() {
  Blynk.virtualWrite(V11, ph1);
  Blynk.virtualWrite(V12, ph2);
  Blynk.virtualWrite(V13, ph3);

  Blynk.virtualWrite(V7, pressure);
}

void onReceive(String key, String value) {
  if (key == "DHT1_Temp") {
    dht1Temp = value.toFloat();
  } else if (key == "DHT1_Hum") {
    dht1Hum = value.toInt();
  } else if (key == "DHT2_Temp") {
    dht2Temp = value.toFloat();
  } else if (key == "DHT2_Hum") {
    dht2Hum = value.toInt();
  } else if (key == "DS18_1") {
    ds18_1 = value.toFloat();
  } else if (key == "DS18_2") {
    ds18_2 = value.toFloat();
  } else if (key == "DS18_3") {
    ds18_3 = value.toFloat();
  } else if (key == "Pressure") {
    pressure = value.toFloat();
  } else if (key == "TDS1") {
    tds1 = value.toFloat();
  } else if (key == "TDS2") {
    tds2 = value.toFloat();
  } else if (key == "TDS3") {
    tds3 = value.toFloat();
  } else if (key == "PH1") {
    ph1 = value.toFloat();
  } else if (key == "PH2") {
    ph2 = value.toFloat();
  } else if (key == "PH3") {
    ph3 = value.toFloat();
  } else if (key == "PUMP") {
    Blynk.virtualWrite(V14, value.toInt());
  } else if (key == "LIGHT") {
    Blynk.virtualWrite(V15, value.toInt());
  }
}
