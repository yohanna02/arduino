#include <WiFi.h>
#include <ThingSpeak.h>
#include <ESPComm.h>

char ssid[] = "OIC_";
char pass[] = "oichub@@1940";

WiFiClient client;

unsigned long myChannelNumber = 3148929;
const char* myWriteAPIKey = "KYUXABYN1VZNRD8O";

int AQI = 0;
int TVOC = 0;
int eCO2 = 0;
byte N = 0;
float CH4 = 0;
float temp = 0;
float humd = 0;

unsigned long prevMillis = 0;

HardwareSerial mySerial(1);
ESPComm esp(mySerial);

void setup() {
  // put your setup code here, to run once:
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  esp.onCommand(processCommand);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(5000);
    }
  }
}

void loop() {
  esp.loop();
  if (millis() - prevMillis > 20000) {
    ThingSpeak.setField(1, CH4);
    ThingSpeak.setField(2, N);
    ThingSpeak.setField(3, TVOC);
    ThingSpeak.setField(4, eCO2);
    ThingSpeak.setField(5, AQI);
    ThingSpeak.setField(6, temp);
    ThingSpeak.setField(7, humd);

    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    prevMillis = millis();
  }
}


void processCommand(String key, String value) {
  if (key == "CH4") {
    CH4 = value.toFloat();
  } else if (key == "N") {
    N = value.toFloat();
  } else if (key == "TVOC") {
    TVOC = value.toFloat();
  } else if (key == "eCO2") {
    eCO2 = value.toFloat();
  } else if (key == "AQI") {
    AQI = value.toFloat();
  } else if (key == "TEMP") {
    temp = value.toFloat();
  } else if (key == "HUMD") {
    humd = value.toFloat();
  }
}
