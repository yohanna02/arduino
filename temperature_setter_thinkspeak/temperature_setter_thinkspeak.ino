#include <LiquidCrystal.h>
#include <DHT.h>
#include <Keypad.h>

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 2, 3, 4, 5 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 6, 7, 8, 9 };     //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal lcd(A5, A4, A3, A2, A1, A0);

DHT dht(10, DHT11);

#define ESP_SERIAL Serial

#define mySSID "OIC"       // WiFi SSID
#define myPWD "oichub@2026"       // WiFi Password
#define myAPI "TWOO3JJIC7YAOR5D"  // API Key
#define myHOST "api.thingspeak.com"

#define FAN 12
#define HEATER 11

unsigned long prevMillis = 0;
unsigned long sendPrevMillis = 0;

float tempC = 0;
float humd = 0;

int set_temp_value = 0;
bool set_temp = true;

void setup() {
  // put your setup code here, to run once:
  pinMode(FAN, OUTPUT);
  pinMode(HEATER, OUTPUT);
  ESP_SERIAL.begin(115200);
  lcd.begin(16, 4);
  dht.begin();

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

  lcd.print(F("Hamidu A.Yahaya"));
  lcd.setCursor(0, 1);
  lcd.print(F("Reg:10/05/04/041"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Stephen J.Bassi"));
  delay(3000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();

  if (key == '#') {
    lcd.clear();
    set_temp = true;
  }

  if (set_temp) {
    String temp_value;
    lcd.setCursor(0, 0);
    lcd.print(F("Set Temp"));

    while (1) {
      char key = keypad.getKey();

      if (key >= '0' && key <= '9') {
        temp_value += key;
        lcd.setCursor(0, 1);
        lcd.print(temp_value);
      } else if (key == '*') {
        lcd.setCursor(0, 1);
        temp_value = "";
        lcd.print(F("                  "));
      } else if (key == '#') {
        set_temp_value = temp_value.toFloat();
        set_temp = false;
        break;
      }
    }
  }

  unsigned long currentMillis = millis();

  if (currentMillis - prevMillis > 2000) {
    tempC = dht.readTemperature();
    humd = dht.readHumidity();

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(F("Temp: "));
    lcd.print(tempC);
    lcd.print(F(" C"));

    lcd.setCursor(0, 1);
    lcd.print(F("Humd: "));
    lcd.print(humd);
    lcd.print(F(" %"));

    if (tempC < set_temp_value) {
      digitalWrite(FAN, LOW);
      digitalWrite(HEATER, HIGH);
    } else {
      digitalWrite(FAN, HIGH);
      digitalWrite(HEATER, LOW);
    }

    prevMillis = millis();
  }

  if (currentMillis - sendPrevMillis > 20000) {
    send_data();
    sendPrevMillis = millis();
  }
}

void send_data() {
  lcd.clear();
  lcd.print(F("Sending data"));
  String sendData = "GET /update?api_key=" myAPI "&field1=" + String(tempC) + "&field2=" + String(humd);
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
