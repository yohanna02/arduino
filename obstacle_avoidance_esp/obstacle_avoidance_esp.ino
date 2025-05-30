/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL21rhrqxsU"
#define BLYNK_TEMPLATE_NAME "spraying robot"
#define BLYNK_AUTH_TOKEN "tYAKIddgnY-TO-VKbzQg7y_DkoUFNkBQ"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "OIC*";
char pass[] = "oichub@@1936";

#define AUTO_MODE "a_m:"
#define MANUAL_MODE "m_m:"

#define FORWARD "f:"
#define BACKWARD "b:"
#define STOP "s:"

#define ON_PUMP "p_o:"
#define OFF_PUMP "p_f"

WidgetLCD lcd(V1);

void setup() {
  // Debug console
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil(':') + ":";

    if (command == AUTO_MODE) {
      lcd.clear();
      lcd.print(0, 0, "AUTO MODE");
    } else if (command == MANUAL_MODE) {
      lcd.clear();
      lcd.print(0, 0, "MANUAL MODE");
    }
  }

  Blynk.run();
}

BLYNK_WRITE(V0) {
  int value = param.asInt();

  lcd.clear();
  lcd.print(0, 0, "Loading...");
  if (value == 1) {
    Serial.print(AUTO_MODE);
  } else {
    Serial.print(MANUAL_MODE);
  }
}

BLYNK_WRITE(V2) {
  int value = param.asInt();

  if (value == 1) {
    Serial.print(FORWARD);
  } else {
    Serial.print(STOP);
  }
}

BLYNK_WRITE(V3) {
  int value = param.asInt();

  if (value == 1) {
    Serial.print(BACKWARD);
  } else {
    Serial.print(STOP);
  }
}

BLYNK_WRITE(V4) {
  int value = param.asInt();

  if (value == 1) {
    Serial.print(ON_PUMP);
  } else {
    Serial.print(OFF_PUMP);
  }
}
