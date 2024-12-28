//#include <LiquidCrystal_I2C.h>
//
//LiquidCrystal_I2C lcd(0x27, 16, 2);

#define NUMBER "+2349022107944"
#define GSM_SERIAL Serial

void setup() {
  // put your setup code here, to run once:
  GSM_SERIAL.begin(9600);
//  lcd.init();
//  lcd.backlight();
//  gsm_init();
  delay(20000);

  send_sms("Hello World!!!", NUMBER);
}

void loop() {
  // put your main code here, to run repeatedly:

}

/*
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
*/

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
