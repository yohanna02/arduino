#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define pin numbers
const int moistureSensorPin = 3;  // Analog pin connected to the moisture sensor
const int pumpPin = 4;             // Digital pin connected to the relay module
const int threshold = 700;         // Threshold value for dry soil (adjust based on your sensor)

#define NUMBER "+2348185507572"
#define GSM_SERIAL Serial

bool sms_sent = false;

void setup() {
  // GSM_SERIAL.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, HIGH);  // Ensure pump is off initially
  // gsm_init();
}

void loop() {
  int moistureLevel = digitalRead(moistureSensorPin);  // Read moisture level

  // if (sms_sent && moistureLevel > threshold) return;

  if (moistureLevel /* > threshold */) {
    lcd.setCursor(0, 0);
    lcd.print("Soil is dry    ");
    lcd.setCursor(0, 1);
    lcd.print("Turning pump ON");

    digitalWrite(pumpPin, LOW);  // Turn on the pump
    sms_sent = true;
    // send_sms("Soil is Dry\nTurning pump ON.", NUMBER);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Soil is moist  ");
    lcd.setCursor(0, 1);
    lcd.print("Turning pump OFF.");
    sms_sent = false;
    digitalWrite(pumpPin, HIGH);  // Turn off the pump
  }

  delay(2000);  // Wait for 2 seconds before the next reading
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
