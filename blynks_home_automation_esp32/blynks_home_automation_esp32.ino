#define BLYNK_TEMPLATE_ID "TMPL2CbVPqXy7"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_AUTH_TOKEN "JvgA1c7BVMkHCaY9nDfHTUZKSDut6iGs"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "yitda";
char pass[] = "12345678";

// Relay Pins
#define RELAY1 32
#define RELAY2 33

// Relay Logic
#define RELAY_ON HIGH
#define RELAY_OFF LOW

//----------------------------------------
// Synchronize relay states after reconnect
//----------------------------------------
BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V0, V1);
}

//----------------------------------------
// Relay 1
//----------------------------------------
BLYNK_WRITE(V0)
{
  int state = param.asInt();

  if(state)
    digitalWrite(RELAY1, RELAY_ON);
  else
    digitalWrite(RELAY1, RELAY_OFF);
}

//----------------------------------------
// Relay 2
//----------------------------------------
BLYNK_WRITE(V1)
{
  int state = param.asInt();

  if(state)
    digitalWrite(RELAY2, RELAY_ON);
  else
    digitalWrite(RELAY2, RELAY_OFF);
}

void setup()
{
  Serial.begin(115200);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);

  // Keep relays OFF on startup
  digitalWrite(RELAY1, RELAY_OFF);
  digitalWrite(RELAY2, RELAY_OFF);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop()
{
  Blynk.run();
}