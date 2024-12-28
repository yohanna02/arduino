// #define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_NAME "Circuit breaker"
#define BLYNK_TEMPLATE_ID "TMPL2aoUqUW7U"
#define BLYNK_AUTH_TOKEN "afscHS0PWVhKGy4g5zZu62FN6W--SRCB"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define ssid "OIC"
#define pass "oichub@2026"

String red_phase_password = "";
String yellow_phase_password = "";
String blue_phase_password = "";

WidgetTerminal terminal(V3);

String pwd_prompt_save = "";
String pwd_prompt = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Blynk.virtualWrite(V0, 1);
  Blynk.virtualWrite(V1, 1);
  Blynk.virtualWrite(V2, 1);

  terminal.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
}

BLYNK_WRITE(V0) {
  int pinValue = param.asInt();

  if (red_phase_password != "" && pinValue == 0) {
    return;
  }

  terminal.clear();
  if (pinValue == 0) {
    terminal.println(F("Set Red phase Password"));
    pwd_prompt_save = "red";
  } else {
    terminal.println(F("Enter Red phase password"));
    pwd_prompt = "red";
  }
  terminal.flush();
}

BLYNK_WRITE(V1) {
  int pinValue = param.asInt();

  if (yellow_phase_password != "" && pinValue == 0) {
    return;
  }

  terminal.clear();
  if (pinValue == 0) {
    terminal.println(F("Set Yellow phase Password"));
    pwd_prompt_save = "yellow";
  } else {
    terminal.println(F("Enter Yellow phase password"));
    pwd_prompt = "yellow";
  }
  terminal.flush();
}

BLYNK_WRITE(V2) {
  int pinValue = param.asInt();

  if (blue_phase_password != "" && pinValue == 0) {
    return;
  }

  terminal.clear();
  if (pinValue == 0) {
    terminal.println(F("Set Blue phase Password"));
    pwd_prompt_save = "blue";
  } else {
    terminal.println(F("Enter Blue phase password"));
    pwd_prompt = "blue";
  }
  terminal.flush();
}

BLYNK_WRITE(V3) {
  terminal.clear();
  if (pwd_prompt_save == "red") {
    red_phase_password = param.asStr();

    Serial.print(F("RED_OFF"));
    terminal.println(F("Red phase password set successfully"));
    pwd_prompt_save = "";
  }
  else if (pwd_prompt == "red") {
    String password = param.asStr();

    if (password == red_phase_password) {
      Serial.print(F("RED_ON"));
      terminal.println(F("Red phase ON"));
      red_phase_password = "";
      pwd_prompt = "";
    } else {
      terminal.println(F("Incorrect password for Red Phase"));
    }
  }

  else if (pwd_prompt_save == "yellow") {
    yellow_phase_password = param.asStr();

    Serial.print(F("YELLOW_OFF"));
    terminal.println(F("Yellow phase password set successfully"));
    pwd_prompt_save = "";
  } 
  else if (pwd_prompt == "yellow") {
    String password = param.asStr();

    if (password == yellow_phase_password) {
      Serial.print(F("YELLOW_ON"));
      terminal.println(F("Yellow phase ON"));
      yellow_phase_password = "";
      pwd_prompt = "";
    } else {
      terminal.println(F("Incorrect password for Yellow Phase"));
    }
  }

  else if (pwd_prompt_save == "blue") {
    blue_phase_password = param.asStr();

    Serial.print(F("BLUE_OFF"));
    terminal.println(F("Blue phase password set successfully"));
    pwd_prompt_save = "";
  } 
  else if (pwd_prompt == "blue") {
    String password = param.asStr();

    if (password == blue_phase_password) {
      Serial.print(F("BLUE_ON"));
      terminal.println(F("Blue phase ON"));
      blue_phase_password = "";
      pwd_prompt = "";
    } else {
      terminal.println(F("Incorrect password for Blue Phase"));
    }
  }
  terminal.flush();
}
