#include <Wire.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

SoftwareSerial gsm(9, 10);

#define GAS A0
#define BUZZER 8

#define GREEN_LED 11
#define RED_LED 12

#define NUMBER "+2349022107944"

#define GSM_SERIAL gsm

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);  // set the LCD address to 0x27 for a 16 chars and 2 line display

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

void setup() {
  GSM_SERIAL.begin(9600);
  pinMode(GAS, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  lcd.begin(16, 2);
  gsm_init();
}

void loop() {
  if (digitalRead(GAS)) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    lcd.setCursor(0, 0);
    lcd.print(F("Gas Detected    "));
    digitalWrite(BUZZER, HIGH);
    send_sms("GAS Detected.", NUMBER);
    // send_sms("GAS Detected.", NUMBER_2);
    delay(10000);
  }
  else {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    lcd.setCursor(0, 0);
    lcd.print(F("Gas Not Detected"));
    digitalWrite(BUZZER, LOW);
  }
}