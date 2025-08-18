#include <LiquidCrystal.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// #include <SPI.h>
// #include <SD.h>
// #include <RTClib.h>
#include <SoftwareSerial.h>

SoftwareSerial espSerial(10, 11);

LiquidCrystal lcd(4, 5, 6, 7, 8, 9);

//RTC/_DS1307 rtc;

DHT dhtOne(1, DHT11);
DHT dhtTwo(0, DHT11);

// #define CHIP_SELECT 10

#define ESP_SERIAL espSerial

#define mySSID "OIC*"              // WiFi SSID
#define myPWD "oichub@@1937"                  // WiFi Password
#define myAPI "4M9S4J9Q0DGNOECS"  // API Key
#define myHOST "api.thingspeak.com"

#define LDR_ONE A4
#define LDR_TWO A5

#define SOIL_MOISTURE_CAP_ONE A3
#define SOIL_MOISTURE_CAP_TWO A2

#define SOIL_MOISTURE_ONE A0
#define SOIL_MOISTURE_TWO A1

#define ONE_WIRE_BUS_ONE 2
#define ONE_WIRE_BUS_TWO 3

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWireOne(ONE_WIRE_BUS_ONE);
OneWire oneWireTwo(ONE_WIRE_BUS_TWO);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensorsOne(&oneWireOne);
DallasTemperature sensorsTwo(&oneWireTwo);

unsigned long prevMillis = 0;
unsigned long sendPrevMillis = 0;

float tempCOne = 0;
float tempCTwo = 0;
float humdOneValue = 0;
float humdTwoValue = 0;
float moistureOneValue = 0;
float moistureTwoValue = 0;
int lightOneValue = 0;
int lightTwoValue = 0;

int state = 0;

void setup() {
  // put your setup code here, to run once:
  ESP_SERIAL.begin(115200);
  pinMode(LDR_ONE, INPUT);
  pinMode(LDR_TWO, INPUT);
  pinMode(SOIL_MOISTURE_CAP_ONE, INPUT);
  pinMode(SOIL_MOISTURE_CAP_TWO, INPUT);
  pinMode(SOIL_MOISTURE_ONE, INPUT);
  pinMode(SOIL_MOISTURE_TWO, INPUT);
  lcd.begin(16, 4);
  dhtOne.begin();
  dhtTwo.begin();
  sensorsOne.begin();
  sensorsTwo.begin();
  // SD.begin(CHIP_SELECT);
//  rtc.begin();

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

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("implementation"));
  delay(3000);
  lcd.clear();

  lcd.print(F("of irrigated"));
  lcd.setCursor(0, 1);
  lcd.print(F("UTC-grade 17"));
  delay(3000);
  lcd.clear();

  lcd.print(F("tomato smart"));
  lcd.setCursor(0, 1);
  lcd.print(F("farm"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Abdullahi Umar"));
  lcd.setCursor(0, 1);
  lcd.print(F("Zakaria"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Reg No"));
  lcd.setCursor(0, 1);
  lcd.print(F("PGD/EE/0102/23"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr.Dr.Usman I"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Hassan"));
  delay(1500);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:

  unsigned long currentMillis = millis();

  if (currentMillis - prevMillis > 5000) {
    sensorsOne.requestTemperatures();
    //    sensorsTwo.requestTemperatures();/
    tempCOne = sensorsOne.getTempCByIndex(0);
    //    tempCTwo = sensorsTwo.getTempCByInde/x(0);

    humdOneValue = dhtOne.readHumidity();
    humdTwoValue = dhtTwo.readHumidity();

    moistureOneValue = map((analogRead(SOIL_MOISTURE_CAP_ONE) + analogRead(SOIL_MOISTURE_ONE)) / 2, 0, 1023, 0, 100);
    moistureTwoValue = map((analogRead(SOIL_MOISTURE_CAP_TWO) + analogRead(SOIL_MOISTURE_TWO)) / 2, 0, 1023, 0, 100);

    lightOneValue = map(analogRead(LDR_ONE), 0, 1023, 0, 100);
    lightTwoValue = map(analogRead(LDR_TWO), 0, 1023, 0, 100);

    lcd.clear();
    if (state == 0) {
      lcd.print(F("Temp 1: "));
      lcd.print(tempCOne);
      lcd.setCursor(0, 1);
      lcd.print(F("Temp 2: "));
      lcd.print(tempCOne);
      lcd.setCursor(0, 2);
      lcd.print(F("L1:"));
      lcd.print(lightOneValue);
      lcd.print(F(" L2:"));
      lcd.print(lightTwoValue);
      state = 1;
    } else if (state == 1) {
      lcd.print(F("H1:"));
      lcd.print(humdOneValue);
      lcd.setCursor(0, 1);
      lcd.print(F("H2:"));
      lcd.print(humdTwoValue);
      lcd.setCursor(0, 2);
      lcd.print(F("M1:"));
      lcd.print(moistureOneValue);
      lcd.setCursor(0, 3);
      lcd.print(F("M2:"));
      lcd.print(moistureTwoValue);
      state = 0;
    }
    prevMillis = millis();
  }

  if (currentMillis - sendPrevMillis > 300000) {
    send_data();
    // logData();
    sendPrevMillis = millis();
  }
}

// void logData() {
//   File dataFile = SD.open("datalog.txt", FILE_WRITE);
// //  DateTime now = rtc.now();

// //  char time[] = "hh:mm";
// //  char date[] = "DD/MM/YYYY";

//   lcd.clear();
//   if (dataFile) {
// //    dataFile.print(F("Time: "));
// //    dataFile.println(now.toString(time));
// //    dataFile.print(F("Date: "));
// //    dataFile.println(now.toString(date));
// //    dataFile.println();

//     dataFile.print(F("Light 1: "));
//     dataFile.println(lightOneValue);
//     dataFile.print(F("Light 2: "));
//     dataFile.println(lightTwoValue);

//     dataFile.print(F("Humidity 1: "));
//     dataFile.println(humdOneValue);
//     dataFile.print(F("Humidity 2: "));
//     dataFile.println(humdTwoValue);

//     dataFile.print(F("Temperature 1: "));
//     dataFile.println(tempCOne);
//     dataFile.print(F("Temperature 2: "));
//     dataFile.println(tempCOne);

//     dataFile.print(F("Moisture 1: "));
//     dataFile.println(moistureOneValue);
//     dataFile.print(F("Moisture 2: "));
//     dataFile.println(moistureTwoValue);

//     dataFile.println();
//     dataFile.println();

//     dataFile.close();

//     lcd.print(F("Data Saved!!!"));
//   } else {
//     lcd.print(F("Error saving"));
//     lcd.setCursor(0, 1);
//     lcd.print(F("to SD card"));
//   }
//   delay(3000);
//   lcd.clear();
// }

void send_data() {
  lcd.clear();
  lcd.print(F("Sending data"));
  String sendData = "GET /update?api_key=" myAPI "&field1=" + String(lightOneValue) + "&field2=" + String(lightTwoValue) + "&field3=" + String(humdOneValue) + "&field4=" + String(humdTwoValue) + "&field5=" + String(tempCOne) + "&field6=" + String(tempCOne) + "&field7=" + String(moistureOneValue) + "&field8=" + String(moistureTwoValue);
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
