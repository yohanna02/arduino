#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#define BUZZER_PIN 12
#define SERVO_PIN 11

LiquidCrystal lcd(13, 10, A0, A1, A2, A3);
Servo doorServo;

// === Keypad Setup ===
const byte ROWS = 4;  // Four rows
const byte COLS = 4;  // Four columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte colPins[COLS] = { 5, 4, 3, 2 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// === Vault Password ===
String password = "1234";  // Change this to your preferred password
String input = "";

void setup() {
  lcd.begin(16, 2);
  doorServo.attach(SERVO_PIN);
  pinMode(BUZZER_PIN, OUTPUT);
  doorServo.write(0);  // Door locked
  lcd.setCursor(0, 0);
  lcd.print("Vault System");
  delay(1500);
  lcd.clear();
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    if (key == '#') {  // Check password
      checkPassword();
    } 
    else if (key == '*') {  // Clear input manually
      clearInput();
    } 
    else {
      // Add key if not special
      if (input.length() < 8) {  // Limit input length
        input += key;
        lcd.print('*');
      }
    }
  }
}

void checkPassword() {
  lcd.clear();
  if (input == password) {
    lcd.print("Access Granted");
    doorServo.write(90);  // Unlock door
    delay(3000);
    doorServo.write(0);   // Lock again
  } else {
    lcd.print("Access Denied!");
    wrongPassword();
  }

  delay(1000);
  clearInput(); // Always clear input and reset after checking
}

void wrongPassword() {
  // Buzzer beeps three times for wrong password
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}

void clearInput() {
  input = "";
  lcd.clear();
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
}
