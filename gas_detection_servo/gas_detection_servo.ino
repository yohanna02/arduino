// #include <Servo.h>
#include <LiquidCrystal.h>
// #include <Keypad.h>
#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::FULL4WIRE, 9, 10, 11, 12);

// const byte ROWS = 4;  //four rows
// const byte COLS = 3;  //three columns
// char keys[ROWS][COLS] = {
//   { '1', '2', '3' },
//   { '4', '5', '6' },
//   { '7', '8', '9' },
//   { '*', '0', '#' }
// };
// byte rowPins[ROWS] = { 9, 10, 11, 12 };  //connect to the row pinouts of the keypad
// byte colPins[COLS] = { 8, 7, 6 };        //connect to the column pinouts of the keypad

// Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

#define GAS A0
// #define SERVO_PIN 3
#define BUZZER 4

String number = "+2348109365567";

#define GSM_SERIAL Serial

LiquidCrystal lcd(A5, A4, A3, A2, A1, 13);
// Servo myServo;

// void setPhoneNumber() {
//   String temp_number = number;
//   while (1) {
//     lcd.setCursor(0, 0);
//     lcd.print(F("Set Number"));
//     lcd.setCursor(0, 1);
//     lcd.print(temp_number);
//     char key = keypad.getKey();

//     if (key >= '0' && key <= '9') {
//       temp_number += key;
//       lcd.setCursor(0, 1);
//       lcd.print(temp_number);
//     } else if (key == '*') {
//       temp_number = "+234";
//     } else if (key == '#') {
//       if (temp_number.length() == 14) {
//         number = temp_number;
//         lcd.clear();
//         lcd.print(F("Number Set"));
//         delay(3000);
//         lcd.clear();
//         break;
//       } else {
//         lcd.clear();
//         lcd.print(F("Invalid Number"));
//         delay(3000);
//         lcd.clear();
//       }
//     }
//   }
// }

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
  // myServo.attach(SERVO_PIN);
  lcd.begin(16, 2);
  // gsm_init();
  // setPhoneNumber();

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("Construction"));
  delay(3000);
  lcd.clear();

  lcd.print(F("of gas leakage"));
  lcd.setCursor(0, 1);
  lcd.print(F("detection system"));
  delay(3000);
  lcd.clear();

  lcd.print(F("with SMS"));
  lcd.setCursor(0, 1);
  lcd.print(F("feedback"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Shittu Umar Faruq"));
  lcd.setCursor(0, 1);
  lcd.print(F("2018/1/00096EE"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr Maika"));
  delay(3000);
  lcd.clear();
}

void loop() {
  // char key = keypad.getKey();
  // if (key == "#") {
  //   setPhoneNumber();
  // }
  if (analogRead(GAS) > 400) {
    lcd.setCursor(0, 0);
    lcd.print(F("Gas Detected    "));
    digitalWrite(BUZZER, HIGH);
    send_sms("GAS Detected!!!", number.c_str());

    stepper.setMaxSpeed(200.0);
    stepper.setAcceleration(100.0);

    stepper.runToNewPosition(1000);

    delay(2000);
    digitalWrite(BUZZER, LOW);
    // int i = 0;
    // for (; i <= 180; i++) {
    //   myServo.write(i);
    //   delay(15);
    // }
    // delay(10000);
    // for (; i >= 0; i--) {
    //   myServo.write(i);
    //   delay(15);
    // }
    // lcd.clear();
  } else {
    lcd.setCursor(0, 0);
    lcd.print(F("Gas Not Detected"));
    digitalWrite(BUZZER, LOW);
  }
}