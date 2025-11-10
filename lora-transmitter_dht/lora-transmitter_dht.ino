#include <LiquidCrystal.h>
#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>

DHT dht(3, DHT11);

LiquidCrystal lcd(A5, A0, A4, A3, A2, A1);

unsigned long prevMillis = 0;

#define SS 10
#define RST 9
#define DIO0 2

void setup() {
  lcd.begin(20, 4);
  dht.begin();
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    lcd.print("Starting LoRa failed!");
    while (1)
      ;
  }

  lcd.print(F("Loading..."));
}

/*--------------------------------------------------------------------------
  MAIN LOOP FUNCTION
  Cylce every 1000ms and perform measurement
 --------------------------------------------------------------------------*/

void loop() {
  if (millis() - prevMillis > 7000) {

    float temp = dht.readTemperature();
    float humd = dht.readHumidity();

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(F("Temp: "));
    lcd.print(temp);
    lcd.print(F("C    "));

    lcd.setCursor(0, 1);
    lcd.print(F("Humd: "));
    lcd.print(humd);
    lcd.print(F("%    "));

    String data = "TEMP=" + String(temp, 1) + ",HUMD=" + String(humd, 1);

    LoRa.beginPacket();
    LoRa.print(data);
    LoRa.endPacket();


    prevMillis = millis();
  }
}
