#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <LoRa.h>
#include <ESPComm.h>

ESPComm esp(Serial);

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define SS 10
#define RST 9
#define DIO0 2

int AQI = 0;
int TVOC = 0;
int eCO2 = 0;
byte N = 0;
float CH4 = 0;
float temp = 0;
float humd = 0;

unsigned long prevMillis = 0;
bool got_data = false;

unsigned long displayMillis = 0;
int state = 0;

void setup() {
  // put your setup code here, to run once:
  esp.begin(9600);

  lcd.init();
  lcd.backlight();

  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    lcd.print("Starting LoRa failed!");
    while (1)
      ;
  }

  lcd.print(F("Waiting for"));
  lcd.setCursor(0, 1);
  lcd.print(F("Data"));
}

void loop() {
  // put your main code here, to run repeatedly:
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String incoming = "";
    while (LoRa.available()) {
      char value = (char)LoRa.read();
      if (value == ',') {
        processCommand(incoming);
        incoming = "";
        continue;
      }
      incoming += value;
    }
    processCommand(incoming);

    got_data = true;
  }

  if (got_data && millis() - displayMillis > 3000) {
    lcd.clear();

    if (state == 0) {
      lcd.setCursor(0, 1);
      lcd.print(F("TVOC: "));
      lcd.print(TVOC);
      lcd.print(F(" ppb    "));

      lcd.setCursor(0, 2);
      lcd.print(F("eCO2: "));
      lcd.print(eCO2);
      lcd.print(F(" ppm    "));

      lcd.setCursor(0, 3);
      lcd.print(F("N:"));
      lcd.print(N);
      lcd.print(F("mg/kg "));

      lcd.print(F("CH4:"));
      lcd.print(CH4);
      lcd.print(F("ppm    "));

      lcd.setCursor(0, 0);
      lcd.print(F("AQI: "));
      lcd.print(AQI);
      lcd.print(F("      "));
    } else {
      lcd.print(F("Temp:"));
      lcd.print(temp);
      lcd.print(F("C"));
      lcd.setCursor(0, 1);
      lcd.print(F("Humd:"));
      lcd.print(humd);
    }
    state++;
    if (state == 2) {
      state = 0;
    }
    displayMillis = millis();
  }

  if (millis() - prevMillis > 10000) {
    // String data = "CH4=" + String(CH4, 1) + ",N=" + String(N) + ",TVOC=" + String(TVOC) + ",eCO2=" + String(eCO2) + ",AQI=" + String(AQI) + ",TEMP=" + String(temp, 1) + ",HUMD=" + String(humd, 1);
    // Serial.print(data);

    esp.send("CH4", CH4);
    delay(200);
    esp.send("N", N);
    delay(200);
    esp.send("TVOC", TVOC);
    delay(200);
    esp.send("eCO2", eCO2);
    delay(200);
    esp.send("AQI", AQI);
    delay(200);
    esp.send("TEMP", temp);
    delay(200);
    esp.send("HUMD", humd);


    prevMillis = millis();
  }

  delay(100);  // Slight delay
}

void processCommand(String cmd) {
  int sepIndex = cmd.indexOf('=');

  if (sepIndex != -1) {
    String key = cmd.substring(0, sepIndex);
    float value = cmd.substring(sepIndex + 1).toFloat();

    if (key == "CH4") {
      CH4 = value;
    } else if (key == "N") {
      N = value;
    } else if (key == "TVOC") {
      TVOC = value;
    } else if (key == "eCO2") {
      eCO2 = value;
    } else if (key == "AQI") {
      AQI = value;
    } else if (key == "TEMP") {
      temp = value;
    } else if (key == "HUMD") {
      humd = value;
    }
  }
}
