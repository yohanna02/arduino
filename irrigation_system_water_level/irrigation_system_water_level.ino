#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

SoftwareSerial esp(7, 8);

#define ESP_SERIAL esp

#define mySSID "OIC"  // WiFi SSID
#define myPWD "oichub@@2027"               // WiFi Password
#define myAPI "EUVJGUPX9VT5PO1W"         // API Key
#define myHOST "api.thingspeak.com"

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions
const int soilMoisturePin = A3;
const int pumpPin = 11;
const int trigPin = 10;
const int echoPin = 9;

// Ultrasonic sensor
const int minWaterLevel = 15;  // Minimum water level in cm

// Keypad setup
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 6, 5, 4, 3 };  // Connect to the row pins
byte colPins[COLS] = { 2, 1, 0 };     // Connect to the column pins
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables
int soilMoistureThreshold = 50;  // Default threshold
String input = "";
bool settingThreshold = true;

unsigned long sendPrevMillis = 0;

void setup() {
  ESP_SERIAL.begin(115200);
  lcd.init();
  lcd.backlight();
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
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(soilMoisturePin, INPUT);

  lcd.clear();
  lcd.print(F("DESIGN AND"));
  lcd.setCursor(0, 1);
  lcd.print(F("CONSTRUCTION"));
  delay(3000);

  lcd.clear();
  lcd.print(F("OF FARM"));
  lcd.setCursor(0, 1);
  lcd.print(F("IRRIGATION SYSTEM"));
  delay(3000);

  lcd.clear();
  lcd.print(F("VIA IOT"));
  delay(3000);

  lcd.clear();
  lcd.print(F("SHIMA T.JEREMIAH"));
  lcd.setCursor(0, 1);
  lcd.print(F("EEE/H/EEC/22/005"));
  delay(3000);

  lcd.clear();
  lcd.print(F("IGOCHE P.ADA"));
  lcd.setCursor(0, 1);
  lcd.print(F("BPU/EEE/H/EEC/006"));
  delay(3000);

  lcd.clear();
  lcd.print(F("ADA ALEXANDER"));
  lcd.setCursor(0, 1);
  lcd.print(F("EEE/H/EEC/22/012"));
  delay(3000);

  lcd.clear();
  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("ENGR.JOHN ODEH"));
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Set M.threshold"));
}

void loop() {
  // Handle keypad input
  char key = keypad.getKey();

  if (key == '#') {
    settingThreshold = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Moisture threshold"));
    digitalWrite(pumpPin, LOW);
  }

  while (settingThreshold) {
    char key = keypad.getKey();
    if (key == '#') {
      // Confirm setting threshold
      if (input != "") {
        soilMoistureThreshold = input.toInt();
        input = "";
        settingThreshold = false;
        lcd.clear();
        lcd.print(F("Threshold set"));
        delay(3000);
        lcd.clear();
      }
    } else if (key == '*') {
      // Start setting threshold
      input = "";
      settingThreshold = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Moisture threshold"));
    } else if (key >= '0' && key <= '9') {
      // Append key to input
      input += key;
      lcd.setCursor(0, 1);
      lcd.print(input);
    }
  }

  // Measure soil moisture
  int soilMoistureValue = analogRead(soilMoisturePin);
  int soilMoisturePercent = map(soilMoistureValue, 0, 1023, 100, 0);

  // Measure water level
  int waterLevel = measureDistance(trigPin, echoPin);

  // Display readings
  lcd.setCursor(0, 0);
  lcd.print(F("Soil: "));
  lcd.print(soilMoisturePercent);
  lcd.print("%    ");

  lcd.setCursor(0, 1);
  // Check conditions to turn on the pump
  if (soilMoisturePercent < soilMoistureThreshold && waterLevel < minWaterLevel) {
    digitalWrite(pumpPin, HIGH);
    lcd.print("Pump ON                  ");
  } else {
    digitalWrite(pumpPin, LOW);
    if (waterLevel >= minWaterLevel) {
      lcd.print("Low water level        ");
    } else {
      lcd.print("Soil moisture ok       ");
    }
  }

  unsigned long currentMillis = millis();
  if (currentMillis - sendPrevMillis > 60000) {
    send_data(String(waterLevel), String(soilMoisturePercent));
    sendPrevMillis = millis();
  }
}

long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;  // Convert to cm
  return distance;
}

void send_data(String waterLevel, String moistureLevel) {
  lcd.clear();
  lcd.print(F("Sending data"));
  String sendData = "GET /update?api_key=" myAPI "&field1=" + waterLevel + "&field2=" + moistureLevel;
  espData("AT+CIPMUX=1", 1000);  // Allow multiple connections
  espData("AT+CIPSTART=0,\"TCP\",\"" myHOST "\",80", 1000);
  espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000);
  ESP_SERIAL.find(">");
  ESP_SERIAL.println(sendData);

  espData("AT+CIPCLOSE=0", 1000);
  delay(2000);
  lcd.clear();
}

void espData(String command, const int timeout) {
  ESP_SERIAL.println(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
  }

  ESP_SERIAL.readString();
}
