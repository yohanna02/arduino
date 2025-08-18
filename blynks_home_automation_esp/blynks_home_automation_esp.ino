// #define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL2AUnNGvEJ"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_AUTH_TOKEN "jZAmLh6vqjiIv0PwPJtEHtvYVVo4JFTS"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "time.h"

#include "command.h"

#define SECRET_SSID "NE_2.4G_345D3F"
#define SECRET_PASS "D392B417"

unsigned long prevMillis = 0;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;   // GMT+1 = 1 * 3600 seconds
const int daylightOffset_sec = 0;  // No daylight saving in Nigeria

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, SECRET_SSID, SECRET_PASS);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');

    processCommand(command);
  }

  if (millis() - prevMillis > 10000) {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      String hour = timeinfo.tm_hour < 10 ? "0" + String(timeinfo.tm_hour) : String(timeinfo.tm_hour);
      String minute = timeinfo.tm_min < 10 ? "0" + String(timeinfo.tm_min) : String(timeinfo.tm_min);
      String currentTime = hour + ":" + minute;
      sendCommand(TIME, currentTime);
    }

    prevMillis = millis();
  }

  Blynk.run();
}

void processCommand(String cmd) {
  int sepIndex = cmd.indexOf('=');

  if (sepIndex != -1) {
    String key = cmd.substring(0, sepIndex);
    float value = cmd.substring(sepIndex + 1).toFloat();

    if (key == ENERGY) {
      Blynk.virtualWrite(V7, value);
    }
  } else {
  }
}

void sendCommand(String key) {
  Serial.print(key);
  Serial.print('\n');
}

void sendCommand(String key, String value) {
  Serial.print(key);
  Serial.print("=");
  Serial.print(value);
  Serial.print('\n');
}

BLYNK_WRITE(V0) {
  int value = param.asInt();

  if (value == 1) {
    sendCommand(FAN_ON);
  } else {
    sendCommand(FAN_OFF);
  }
}

BLYNK_WRITE(V1) {
  int value = param.asInt();

  if (value == 1) {
    sendCommand(BUZZER_ON);
  } else {
    sendCommand(BUZZER_OFF);
  }
}

BLYNK_WRITE(V2) {
  int value = param.asInt();

  if (value == 1) {
    sendCommand(SECURITY_ON);
  } else {
    sendCommand(SECURITY_OFF);
  }
}

BLYNK_WRITE(V3) {
  int value = param.asInt();

  if (value == 1) {
    sendCommand(SOCKET_ON);
  } else {
    sendCommand(SOCKET_OFF);
  }
}

BLYNK_WRITE(V4) {
  int value = param.asInt();

  if (value == 1) {
    sendCommand(ROOM_ON);
  } else {
    sendCommand(ROOM_OFF);
  }
}

BLYNK_WRITE(V5) {
  int value = param.asInt();

  if (value == 1) {
    sendCommand(STORE_ON);
  } else {
    sendCommand(STORE_OFF);
  }
}

BLYNK_WRITE(V6) {
  int value = param.asInt();

  if (value == 1) {
    sendCommand(HUMIDIFIER_ON);
  } else {
    sendCommand(HUMIDIFIER_OFF);
  }
}
