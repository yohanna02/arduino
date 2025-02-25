#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

#define seaLevelPressure_hPa 616

Adafruit_BMP085 bmp;

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 12, 11, 10, 9 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 8, 7, 6, 5 };     //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 16, 2);

String number = "+234";
#define GSM_SERIAL Serial

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

DHT dht(3, DHT11);

#define SOIL_MOISTURE A3
#define LDR A0

// Global variables for timing
unsigned long lastSendTime = 0;
const unsigned long interval = 300000;  // 5 minutes in milliseconds

bool english = true;

String jk = "";
String mass = "";

int state = 0;
unsigned long displayChange = 0;

float dhtHumidity;
float dhtTemp;
float pressure;
float altitude;
float dsTemp;
int soilMoisture;
int ldrValue;

void setup() {
  // put your setup code here, to run once:
  pinMode(SOIL_MOISTURE, INPUT);
  pinMode(LDR, INPUT);
  GSM_SERIAL.begin(9600);
  lcd.init();
  lcd.backlight();
  dht.begin();
  sensors.begin();
  gsm_init();
  receive_mode();
  GSM_SERIAL.readString();

  lcd.print(F("Development of"));
  lcd.setCursor(0, 1);
  lcd.print(F("a Farm data"));
  delay(3000);
  lcd.clear();
  lcd.print(F("monitoring system"));
  lcd.setCursor(0, 1);
  lcd.print(F("with SMS feedback"));
  delay(3000);
  lcd.clear();

  if (!bmp.begin()) {
    lcd.clear();
    lcd.print("BMP Error");
    while (1) {}
  }

  setPhoneNumber();

  lcd.clear();
  lcd.print(F("Set Language"));
  delay(2000);
  lcd.clear();
  lcd.print(F("1. English"));
  lcd.setCursor(0, 1);
  lcd.print(F("2. Hausa"));

  char key;
  unsigned long start = millis();
  while (1) {
    key = keypad.waitForKey();

    if (key == '1' || key == '2') {
      if (key == '2') {
        english = false;
      }
      break;
    }

    if (millis() - start > 20000) {
      break;
    }
  }

  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();
  if (key == '#') {
    setPhoneNumber();
  } else if (key == '*') {
    english != english;

    lcd.clear();
    if (english) {
      lcd.print(F("English"));
    } else {
      lcd.print(F("Hausa"));
    }
    delay(2000);
    lcd.clear();
  }

  if (millis() - displayChange > 3000) {
    // Read all sensors
    dhtHumidity = dht.readHumidity();
    dhtTemp = dht.readTemperature();
    pressure = bmp.readPressure();
    altitude = bmp.readAltitude();
    sensors.requestTemperatures();
    dsTemp = sensors.getTempCByIndex(0);
    soilMoisture = analogRead(SOIL_MOISTURE);
    ldrValue = analogRead(LDR);

    // Display on LCD
    lcd.clear();

    if (state == 0) {
      lcd.print("Temp:" + String(dhtTemp) + "C");
      lcd.setCursor(0, 1);
      lcd.print("Hum:" + String(dhtHumidity) + "%");
    } else if (state == 1) {
      lcd.print("Press:" + String(pressure, 0) + "Pa");
      lcd.setCursor(0, 1);
      lcd.print("Alt:" + String(altitude, 1) + "m");
    } else if (state == 2) {
      lcd.print("Soil Moist:" + String(soilMoisture));
      lcd.setCursor(0, 1);
      lcd.print("LDR:" + String(ldrValue));
    } else if (state == 3) {
      lcd.print("Soil T:" + String(dsTemp, 1) + "C");
    }

    state++;
    if (state > 3) {
      state = 0;
    }
    displayChange = millis();
  }

  // SMS sending logic
  unsigned long currentMillis = millis();
  if (currentMillis - lastSendTime >= interval) {
    String smsData;
    if (english) {
      smsData = "Environmental Monitoring\n\nTemperature: " + String(dhtTemp, 1) + "C\n";
      smsData += "Humidity: " + String(dhtHumidity, 0) + "%\n";
      smsData += "Pressure: " + String(pressure, 0) + "hPa\n";
      smsData += "Altitude: " + String(altitude, 1) + "m\n";
      smsData += "Soil Moisture: " + String(soilMoisture) + "\n";
      smsData += "Light: " + String(ldrValue) + "\n";
      smsData += "Soil Temperature: " + String(dsTemp, 1) + "C";
    } else {
      smsData = "Kula da Muhalli\n\nZafin Yanaye: " + String(dhtTemp, 1) + "C\n";
      smsData += "Danshi: " + String(dhtHumidity, 0) + "%\n";
      smsData += "Matsin Iska: " + String(pressure, 0) + "hPa\n";
      smsData += "Tsayi: " + String(altitude, 1) + "m\n";
      smsData += "Danshin Kasa: " + String(soilMoisture) + "\n";
      smsData += "Haske: " + String(ldrValue) + "\n";
      smsData += "Zafin Kasa: " + String(dsTemp, 1) + "C";
    }
    send_sms(smsData.c_str(), number.c_str());
    receive_mode();
    GSM_SERIAL.readString();
    lastSendTime = currentMillis;
  }

  if (GSM_SERIAL.available()) {
    String smsData;
    if (english) {
      smsData = "Environmental Monitoring\n\nTemp: " + String(dhtTemp, 1) + "C\n";
      smsData += "Humidity: " + String(dhtHumidity, 0) + "%\n";
      smsData += "Pressure: " + String(pressure, 0) + "hPa\n";
      smsData += "Altitude: " + String(altitude, 1) + "m\n";
      smsData += "Soil Moisture: " + String(soilMoisture) + "\n";
      smsData += "Light: " + String(ldrValue) + "\n";
      smsData += "Soil Temp: " + String(dsTemp, 1) + "C";
    } else {
      smsData = "Kula da Muhalli\n\nTemp: " + String(dhtTemp, 1) + "C\n";
      smsData += "Danshi: " + String(dhtHumidity, 0) + "%\n";
      smsData += "Matsin lamba: " + String(pressure, 0) + "hPa\n";
      smsData += "Tsayi: " + String(altitude, 1) + "m\n";
      smsData += "Danshin Kasa: " + String(soilMoisture) + "\n";
      smsData += "Haske: " + String(ldrValue) + "\n";
      smsData += "Yanayin Kasa: " + String(dsTemp, 1) + "C";
    }

    delay(2000);
    while (GSM_SERIAL.available()) {
      char massA = GSM_SERIAL.read();
      mass += massA;
    }
    delay(1000);
    mass.trim();
    delay(30);
    mass.toUpperCase();
    delay(30);
    if (mass.indexOf("STATUS") > -1) {
      GSM_SERIAL.readString();
      lcd.clear();
      lcd.print("Sending Status");
      send_sms(smsData.c_str(), number.c_str());
      delay(3000);
      lcd.clear();
      receive_mode();
      GSM_SERIAL.readString();
      lastSendTime = currentMillis;
    }
    mass = "";
  }
}

void setPhoneNumber() {
  String temp_number = number;
  while (1) {
    lcd.setCursor(0, 0);
    lcd.print(F("Set Number"));
    lcd.setCursor(0, 1);
    lcd.print(temp_number);
    char key = keypad.getKey();

    if (key >= '0' && key <= '9') {
      temp_number += key;
      lcd.setCursor(0, 1);
      lcd.print(temp_number);
    } else if (key == '*') {
      temp_number = "+234";
      lcd.clear();
    } else if (key == '#') {
      if (temp_number.length() == 14) {
        number = temp_number;
        lcd.clear();
        lcd.print(F("Number Set"));
        delay(3000);
        lcd.clear();
        break;
      } else {
        lcd.clear();
        lcd.print(F("Invalid Number"));
        delay(3000);
        lcd.clear();
      }
    }
  }
}

void gsm_init() {
  lcd.clear();
  lcd.print(F("Finding Module.."));
  boolean at_flag = 1;
  while (at_flag) {
    GSM_SERIAL.println("AT");
    while (GSM_SERIAL.available() > 0) {
      if (GSM_SERIAL.find("OK"))
        at_flag = 0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print(F("Module Connected.."));
  delay(500);
  lcd.clear();
  // lcd.print("Disabling ECHO");
  bool echo_flag = 1;
  while (echo_flag) {
    GSM_SERIAL.println("ATE0");
    while (GSM_SERIAL.available() > 0) {
      if (GSM_SERIAL.find("OK"))
        echo_flag = 0;
    }
    delay(1000);
  }
  // lcd.clear();
  //  lcd.print("Echo OFF");
  delay(500);
  lcd.clear();
  lcd.print("Finding Network..");
  bool net_flag = 1;
  while (net_flag) {
    GSM_SERIAL.println("AT+CPIN?");
    while (GSM_SERIAL.available() > 0) {
      if (GSM_SERIAL.find("+CPIN: READY"))
        net_flag = 0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print("Network Found..");
  delay(500);
  lcd.clear();
}

void send_sms(const char *sms_data, const char *number) {
  GSM_SERIAL.println("AT+CMGF=1");
  delay(1000);
  char num_data[26];
  sprintf(num_data, "AT+CMGS=\"%s\"", number);
  GSM_SERIAL.println(num_data);
  delay(1000);
  GSM_SERIAL.print(sms_data);
  delay(1000);
  GSM_SERIAL.write(26);
  delay(1000);
}

void receive_mode() {
  GSM_SERIAL.println("AT+CLVL=0");
  delay(2000);
  GSM_SERIAL.println("AT+CRSL=0");
  delay(2000);
  GSM_SERIAL.println("AT+CMGDA=6");
  delay(5000);
  GSM_SERIAL.println("AT+CMGF=1");
  delay(1000);
  GSM_SERIAL.println("AT+CNMI=1,2,0,0,0");
}
