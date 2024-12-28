#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 5, 6, 7, 8);

#define ESP_SERIAL Serial

#define mySSID "SSID"             // WiFi SSID
#define myPWD ""         // WiFi Password
#define myAPI "V1J1N9KVOBVPEQW2"  // API Key
#define myHOST "api.thingspeak.com"

#define SOUND A1
#define GAS A0
#define BUZZER 4

unsigned long previous_millis = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(SOUND, INPUT);
  pinMode(GAS, INPUT);
  pinMode(BUZZER, OUTPUT);
  ESP_SERIAL.begin(115200);
  lcd.begin(16, 2);

  espData("AT+RST", 1000);                                 //Reset the ESP8266 module
  espData("AT+CWMODE=1", 1000);                            //Set the ESP mode as station mode
  espData("AT+CWJAP=\"" mySSID "\",\"" myPWD "\"", 1000);  //Connect to WiFi network
    lcd.setCursor(0, 0);
    lcd.print("Connecting...");
  delay(3000);
  lcd.clear();
  lcd.print(F("WiFi Connected"));
  delay(1000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - previous_millis > 20000) {
    send_data();
    previous_millis = millis();
  }

  lcd.setCursor(0, 0);
  lcd.print(F("GAS: "));
  lcd.print(analogRead(GAS));
  lcd.print(F("   "));

  if (analogRead(GAS) > 512) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  lcd.setCursor(0, 1);
  lcd.print(String(digitalRead(SOUND) ? "Noise Detected           " : "No Noise            "));
}

void send_data() {
  String sendData = "GET /update?api_key=" myAPI "&field1=" + String(analogRead(GAS)) + "&field2=" + String(digitalRead(SOUND));
  espData("AT+CIPMUX=1", 1000);  // Allow multiple connections
  espData("AT+CIPSTART=0,\"TCP\",\"" myHOST "\",80", 1000);
  espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000);
  ESP_SERIAL.find(">");
  ESP_SERIAL.println(sendData);

  espData("AT+CIPCLOSE=0", 1000);
  delay(2000);
}

void espData(String command, const int timeout) {
  ESP_SERIAL.println(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
  }

  ESP_SERIAL.readString();
}