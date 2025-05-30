// #define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL2AUnNGvEJ"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_AUTH_TOKEN "jZAmLh6vqjiIv0PwPJtEHtvYVVo4JFTS"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include "command.h"

#define SECRET_SSID "NE_2.4G_345D3F"
#define SECRET_PASS "D392B417"

unsigned long prevMillis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, SECRET_SSID, SECRET_PASS);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');

    processCommand(command);
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

// void readState() {
//   ThingSpeak.readMultipleFields(CHANNEL_ID, READ_API_KEY);

//   if (ThingSpeak.getFieldAsInt(1) != fan) {
//     fan = ThingSpeak.getFieldAsInt(1);
//     fan == 1 ? sendCommand(FAN_ON) : sendCommand(FAN_OFF);
//   }

//   if (ThingSpeak.getFieldAsInt(2) != buzzer) {
//     buzzer = ThingSpeak.getFieldAsInt(2);
//     buzzer == 1 ? sendCommand(BUZZER_ON) : sendCommand(BUZZER_OFF);
//   }

//   if (ThingSpeak.getFieldAsInt(3) != securityBulb) {
//     securityBulb = ThingSpeak.getFieldAsInt(3);
//     securityBulb == 1 ? sendCommand(SECURITY_ON) : sendCommand(SECURITY_OFF);
//   }

//   if (ThingSpeak.getFieldAsInt(4) != socket) {
//     socket = ThingSpeak.getFieldAsInt(4);
//     socket == 1 ? sendCommand(SOCKET_ON) : sendCommand(SOCKET_OFF);
//   }

//   if (ThingSpeak.getFieldAsInt(5) != roomBulb) {
//     roomBulb = ThingSpeak.getFieldAsInt(5);
//     roomBulb == 1 ? sendCommand(ROOM_ON) : sendCommand(ROOM_OFF);
//   }

//   if (ThingSpeak.getFieldAsInt(6) != storeBulb) {
//     storeBulb = ThingSpeak.getFieldAsInt(6);
//     storeBulb == 1 ? sendCommand(STORE_ON) : sendCommand(STORE_OFF);
//   }

//   if (ThingSpeak.getFieldAsInt(7) != humidifier) {
//     humidifier = ThingSpeak.getFieldAsInt(7);
//     humidifier == 1 ? sendCommand(HUMIDIFIER_ON) : sendCommand(HUMIDIFIER_OFF);
//   }

//   if (ThingSpeak.getFieldAsInt(8) != energy) {
//     energy = ThingSpeak.getFieldAsFloat(8);
//   }
// }

// void sendState() {
//   ThingSpeak.setField(1, fan);
//   ThingSpeak.setField(2, buzzer);
//   ThingSpeak.setField(3, securityBulb);
//   ThingSpeak.setField(4, socket);
//   ThingSpeak.setField(5, roomBulb);
//   ThingSpeak.setField(6, storeBulb);
//   ThingSpeak.setField(7, humidifier);
//   ThingSpeak.setField(8, energy);
// }

void sendCommand(String key) {
  Serial.print(key);
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
