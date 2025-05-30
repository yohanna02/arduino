#include <SoftwareSerial.h>

SoftwareSerial _serial(2, 3);

#define BLUETOTH_SERIAL _serial

void setup() {
  // put your setup code here, to run once:
  BLUETOTH_SERIAL.begin(9600);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (BLUETOTH_SERIAL.available()) {
    Serial.println(BLUETOTH_SERIAL.readString());
  }
}
