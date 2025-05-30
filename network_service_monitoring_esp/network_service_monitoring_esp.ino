/*
  WriteMultipleFields
  
  Description: Writes values to fields 1,2,3,4 and status in a single ThingSpeak update every 20 seconds.
  
  Hardware: ESP8266 based boards
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
  - Requires ESP8266WiFi library and ESP8622 board add-on. See https://github.com/esp8266/Arduino for details.
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  
  Copyright 2020, The MathWorks, Inc.
*/

#include <ESP8266WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h"  // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password
WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char* myWriteAPIKey = SECRET_WRITE_APIKEY;


void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      delay(5000);
    }
  }
}

void loop() {
  if (Serial.available()) {
    float tempC = Serial.readStringUntil(':').toFloat();
    int fuelLevel = Serial.readStringUntil(':').toInt();
    float voltage = Serial.readStringUntil(':').toFloat();
    float current = Serial.readString().toFloat();

    ThingSpeak.setField(1, tempC);
    ThingSpeak.setField(2, fuelLevel);
    ThingSpeak.setField(3, voltage);
    ThingSpeak.setField(4, current);

    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  }
}
