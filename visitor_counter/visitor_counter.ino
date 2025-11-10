#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------- LCD ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change to 0x3F if needed

// ---------------- KEYPAD ----------------
// 4x3 keypad layout
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

// Keypad pin wiring: 1,2,7,8,A0,A1,A2
byte rowPins[ROWS] = { A2, A1, A0, 8 };
byte colPins[COLS] = { 7, 1, 2 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------------- ULTRASONIC ----------------
#define TRIG_IN 6
#define ECHO_IN 5
#define TRIG_OUT 3
#define ECHO_OUT 4

// ---------------- MOTOR ----------------
#define MOTOR_IN1 9
#define MOTOR_IN2 10
#define MOTOR_PWM 11

#define BUZZER 12

int visitorCount = 0;
int maxVisitors = 0;

long readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 20000);  // 20ms timeout
  long distance = duration * 0.034 / 2;
  return distance;
}

void motorOpen() {
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_PWM, 55);
  lcd.setCursor(0, 1);
  lcd.print("Door Open       ");
  delay(400);
  motorStop();
}

void motorClose() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);
  analogWrite(MOTOR_PWM, 55);
  lcd.setCursor(0, 1);
  lcd.print("Door Closed     ");
  delay(400);
  motorStop();
}

void motorStop() {
  analogWrite(MOTOR_PWM, 0);
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
}

void updateLCD() {
  lcd.setCursor(0, 0);
  lcd.print("Visitors:");
  lcd.print(visitorCount);
  lcd.print("/");
  lcd.print(maxVisitors);
  lcd.print("   ");  // clear extra chars
}

void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(TRIG_IN, OUTPUT);
  pinMode(ECHO_IN, INPUT);
  pinMode(TRIG_OUT, OUTPUT);
  pinMode(ECHO_OUT, INPUT);

  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("implementation of"));
  delay(3000);
  lcd.clear();

  lcd.print(F("bidirectional"));
  lcd.setCursor(0, 1);
  lcd.print(F("visitor counter"));
  delay(3000);
  lcd.clear();

  lcd.print(F("DANIEL ADACHE MICAH"));
  lcd.setCursor(0, 1);
  lcd.print(F("EEP/23/10012"));
  delay(3000);
  lcd.clear();

  lcd.print(F("EDEH ADOTSE VICTOR"));
  delay(3000);
  lcd.clear();

  lcd.print(F("AZENDA JACOB HANMAOR"));
  lcd.setCursor(0, 1);
  lcd.print(F("EEP/23/10005"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr.Helen Adoyi"));
  delay(3000);
  lcd.clear();


  lcd.setCursor(0, 0);
  lcd.print("Set Max Visitors");
  lcd.setCursor(0, 1);

  // Get maxVisitors from keypad
  String input = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        maxVisitors = input.toInt();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Max Set: ");
        lcd.print(maxVisitors);
        delay(1500);
        break;
      } else if (key >= '0' && key <= '9') {
        input += key;
        lcd.print(key);
      }
    }
  }

  lcd.clear();
  updateLCD();
  lcd.setCursor(0, 1);
  lcd.print("System Ready   ");
}

void loop() {
  long distIn = readDistance(TRIG_IN, ECHO_IN);
  long distOut = readDistance(TRIG_OUT, ECHO_OUT);

  // lcd.print(distIn);
  // lcd.print(F("  "));
  // lcd.print(distOut);
  // lcd.print(F("  "));

  // Entrance detection
  if (distIn > 0 && distIn < 20) {
    if (visitorCount < maxVisitors) {
      visitorCount++;
      updateLCD();
      motorOpen();
      delay(3000);  // allow person to pass
      motorClose();
      updateLCD();
    } else {
      lcd.setCursor(0, 1);
      digitalWrite(BUZZER, HIGH);
      lcd.print("Entry Denied    ");
      delay(1000);
      digitalWrite(BUZZER, LOW);
    }
  }

  // Exit detection
  if (distOut > 0 && distOut < 20) {
    if (visitorCount > 0) {
      visitorCount--;
      updateLCD();
      motorOpen();
      delay(3000);
      motorClose();
      updateLCD();
    }
  }

  delay(200);
}
