#include <LiquidCrystal.h>
#include <ESPComm.h>
#include <DHT.h>

DHT dht(2, DHT11);

ESPComm esp(Serial);

String mySSID = "Galaxy A14";           // WiFi SSID
String myPWD = "Marthana04";        // WiFi Password
String myAPI1 = "05KBPDG721Q1QH9N";  //WRIRE KEY FOR CHANNEL 1
//"XQARQ2I9Y3OIU7E8";   // API Key
String myHOST = "api.thingspeak.com";
String myPORT = "80";

const int rs = 5, en = 6, d4 = 7, d5 = 8, d6 = 9, d7 = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int T = 600000;
int s1p1 = A0, s2p1 = A1, s3p1 = A2, s4p1 = A3;
float tempc;  //variable to store temperature in degree Celsius
unsigned long prev_millis = 0;
void setup() {  // put your setup code here, to run once:
  pinMode(s1p1, INPUT);
  pinMode(s2p1, INPUT);
  pinMode(s3p1, INPUT);
  pinMode(s4p1, INPUT);
  lcd.begin(16, 2);
  esp.begin(9600);
  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print(F("DESIGN AND"));
  lcd.setCursor(0, 1);
  lcd.print(F("CONSTRUCTION"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("OF A REAL TIME"));
  lcd.setCursor(0, 1);
  lcd.print(F("IOT BASED BATTERY"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("BANK MONITORING"));
  lcd.setCursor(0, 1);
  lcd.print(F("SYSTEM"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Oluwaseyifunmi Martins Obolo"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/152173"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Stephen Rebecca Tsok"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/152447"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr.D.M.Nazif"));
  delay(3000);
  lcd.clear();

  prev_millis = millis();
}
void loop() {
  float s1p1_value = analogRead(s1p1);
  s1p1_value = (4.2 * s1p1_value) / 1023;
  float s2p1_value = analogRead(s2p1);
  s2p1_value = (8.4 * s2p1_value) / 1023;
  float s3p1_value = analogRead(s3p1);
  s3p1_value = (12.6 * s3p1_value) / 1023;
  float s4p1_value = analogRead(s4p1);
  s4p1_value = (16.8 * s4p1_value) / 1023;

  float temp = dht.readTemperature();
  float humd = dht.readHumidity();

  lcd.setCursor(0, 0);
  lcd.print("s1=" + String(s1p1_value) + "V ");

  lcd.print("s2=" + String(s2p1_value) + "V   ");
  lcd.setCursor(0, 1);
  lcd.print("s3=" + String(s3p1_value) + "V ");

  lcd.print("s4=" + String(s4p1_value) + "V   ");
  delay(3000);
  lcd.clear();
  lcd.print(F("Temp: "));
  lcd.print(temp);
  lcd.setCursor(0, 1);
  lcd.print(F("Humd: "));
  lcd.print(humd);
  delay(3000);
  lcd.clear();
  if (millis() - prev_millis > T) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Updating");
    lcd.setCursor(0, 1);
    lcd.print("Thingspeak");
    
    esp.send("s1", s1p1_value);
    delay(200);
    esp.send("s2", s2p1_value);
    delay(200);
    esp.send("s3", s3p1_value);
    delay(200);
    esp.send("s4", s4p1_value);
    delay(200);
    esp.send("temp", temp);
    delay(200);
    esp.send("humd", humd);

    prev_millis = millis();
    lcd.clear();
  }
}
void espData(String command, const int timeout) {
  Serial.println(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (Serial.available()) {
      Serial.readString();
    }
  }
}