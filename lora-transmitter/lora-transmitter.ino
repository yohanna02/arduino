#include <LiquidCrystal.h>
#include <Wire.h>
#include <DFRobot_ENS160.h>
#include <SPI.h>
#include <LoRa.h>

#define MQ4 A0
#define NPK_SERIAL Serial3

#define RE 6
#define DE 5

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

DFRobot_ENS160_I2C ens160;

const byte nitro[] = { 0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c };
byte values[11];

unsigned long prevMillis = 0;
int state = 0;

int AQI = 0;
int TVOC = 0;
int eCO2 = 0;
byte N = 0;
float CH4 = 0;

// MQ-4 parameters
#define RL 10000
#define Ro 9000


// --- Averaged analog read ---
int readAnalogAvg(int pin, int samples = 10) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / samples;
}

void setup() {
  lcd.begin(20, 4);
  NPK_SERIAL.begin(9600);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  // ENS160 init
  if (!ens160.begin()) {
    lcd.clear();
    lcd.print("ENS160 failed");
    while (1)
      ;
  }
  ens160.setPWRMode(ENS160_STANDARD_MODE);
  ens160.setTempAndHum(25.0, 50.0);  // Optional: set environment data

  LoRa.setPins(53, A4, 2);
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
  if (millis() - prevMillis > 2000) {

    AQI = ens160.getAQI();
    TVOC = ens160.getTVOC();
    eCO2 = ens160.getECO2();

    N = nitrogen();

    int mq4Raw = readAnalogAvg(MQ4, 5);
    CH4 = getMethanePPM(mq4Raw);

    lcd.clear();

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

    String data = "CH4=" + String(CH4, 1) + ",N=" + String(N) + ",TVOC=" + String(TVOC) + ",eCO2=" + String(eCO2) + ",AQI=" + String(AQI);

    LoRa.beginPacket();
    LoRa.print(data);
    LoRa.endPacket();
    

    prevMillis = millis();
  }
}

// --- Methane calculation from MQ-4 ---
float getMethanePPM(int analogValue) {
  float Vout = analogValue * (5.0 / 1023.0);
  float Rs = (5.0 - Vout) * RL / Vout;
  float ratio = Rs / Ro;
  float ppm_log = -0.318 * log10(ratio) + 1.133;
  return pow(10, ppm_log);
}

byte nitrogen() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if (NPK_SERIAL.write(nitro, sizeof(nitro)) == 8) {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for (byte i = 0; i < 7; i++) {
      //Serial.print(NPK_SERIAL.read(),HEX);
      values[i] = NPK_SERIAL.read();
      Serial.print(values[i], HEX);
    }
    Serial.println();
  }
  return values[4];
}
