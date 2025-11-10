#include <Wire.h>
#include <LiquidCrystal.h>

// ===== GSM SETUP =====
#define GSM_SERIAL Serial
#define NUMBER "+2349124255416"
#define NUMBER2 "+2348035301331"

// ===== PIN DEFINITIONS =====
#define PIR A4
#define GAS A3
#define BUZZER 8

// ===== LCD (RS, EN, D4, D5, D6, D7) =====
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// ===== STATE =====
bool intruder_sms_sent = false;
bool gas_sms_sent = false;

// ===== FUNCTIONS =====
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
  lcd.print(F("Module Connected"));
  delay(500);

  bool echo_flag = 1;
  while (echo_flag) {
    GSM_SERIAL.println("ATE0");
    while (GSM_SERIAL.available() > 0) {
      if (GSM_SERIAL.find("OK"))
        echo_flag = 0;
    }
    delay(1000);
  }

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
  char num_data[30];
  sprintf(num_data, "AT+CMGS=\"%s\"", number);
  GSM_SERIAL.println(num_data);
  delay(1000);
  GSM_SERIAL.print(sms_data);
  delay(1000);
  GSM_SERIAL.write(26);
  delay(1000);
}

// ===== SETUP =====
void setup() {
  GSM_SERIAL.begin(9600);

  pinMode(PIR, INPUT);
  pinMode(GAS, INPUT);

  pinMode(BUZZER, OUTPUT);

  lcd.begin(16, 2);

  // Intro message
  lcd.print(F("GSM BASED HOME"));
  lcd.setCursor(0, 1);
  lcd.print(F("SECURITY SYSTEM"));
  delay(3000);
  lcd.clear();

  lcd.print(F("WITH INTRUDER"));
  lcd.setCursor(0, 1);
  lcd.print(F("ALERT"));
  delay(3000);
  lcd.clear();

  lcd.print(F("ISAH IBRAHIM.MUHD"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/150319"));
  delay(3000);
  lcd.clear();

  lcd.print(F("KEVIN GYANG SIMON"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/150027"));
  delay(3000);
  lcd.clear();

  lcd.print(F("MUNKAILA N.YAKUBU"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/149996"));
  delay(3000);
  lcd.clear();

  lcd.print(F("AMINU AHMAD YAKUBU"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/150028"));
  delay(3000);
  lcd.clear();

  lcd.print(F("LEVI IBRAHIM"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/150213"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("ENGR.Aliyu Abdurahman"));
  delay(3000);
  lcd.clear();

  // gsm_init();
}

// ===== LOOP =====
void loop() {
  int pir_value = digitalRead(PIR);
  int gas_value = analogRead(GAS);

  // lcd.setCursor(0, 0);
  // lcd.print(F("GAS: "));
  // lcd.print(gas_value);
  // lcd.print(F("   "));

  // lcd.setCursor(0, 1);
  // lcd.print(F("PIR: "));
  // lcd.print(pir_value);
  // lcd.print(F("   "));

  // ==== Intruder detection ====
  if (pir_value) {
    digitalWrite(BUZZER, HIGH);
    lcd.setCursor(0, 0);
    lcd.print(F("Intruder Detected "));
    if (!intruder_sms_sent) {
      send_sms("Intruder Detected!!!", NUMBER);
      delay(1000);
      send_sms("Intruder Detected!!!", NUMBER2);
      intruder_sms_sent = true;
    }
  } else {
    digitalWrite(BUZZER, LOW);
    lcd.setCursor(0, 0);
    lcd.print(F("No Intruder       "));
    intruder_sms_sent = false;
  }

  // ==== Gas detection ====
  if (gas_value >= 150) {
    digitalWrite(BUZZER, HIGH);
    lcd.setCursor(0, 1);
    lcd.print(F("Gas Detected     "));

    if (!gas_sms_sent) {
      send_sms("GAS Detected!!!", NUMBER);
      delay(1000);
      send_sms("GAS Detected!!!", NUMBER2);
      gas_sms_sent = true;
    }
  } else {
    digitalWrite(BUZZER, LOW);
    lcd.setCursor(0, 1);
    lcd.print(F("Gas Not Detected "));
    gas_sms_sent = false;
  }

  delay(500);
}
