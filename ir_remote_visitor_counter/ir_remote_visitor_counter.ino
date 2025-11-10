#include <IRremote.h>
#include <LiquidCrystal_I2C.h>

// --- IR Receiver ---
#define IR_PIN 6
// ---------------- ULTRASONIC ----------------
#define TRIG_IN 4
#define ECHO_IN 5
#define TRIG_OUT 2
#define ECHO_OUT 3

// ---------------- MOTOR ----------------
#define MOTOR_R 11
#define MOTOR_L 10

// #define BUZZER 12

#define OPEN_COMMAND 0x45
#define CLOSE_COMMAND 0x47

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool opened = false;
int visitorCount = 0;

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();

  IrReceiver.begin(IR_PIN);

  pinMode(TRIG_IN, OUTPUT);
  pinMode(ECHO_IN, INPUT);
  pinMode(TRIG_OUT, OUTPUT);
  pinMode(ECHO_OUT, INPUT);

  pinMode(MOTOR_R, OUTPUT);
  pinMode(MOTOR_L, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (IrReceiver.decode()) {
    // --- Control Loads ---
    if (IrReceiver.decodedIRData.command == OPEN_COMMAND) {
      opened = true;
      motorOpen();
    } else if (IrReceiver.decodedIRData.command == CLOSE_COMMAND) {
      opened = false;
      motorClose();
    }

    IrReceiver.resume();
  }

  lcd.setCursor(0, 0);
  lcd.print(F("Visitors: "));
  lcd.print(visitorCount);
  lcd.print(F("  "));

  long distIn = readDistance(TRIG_IN, ECHO_IN);
  long distOut = readDistance(TRIG_OUT, ECHO_OUT);

  if (!opened) return;

  if (distIn > 0 && distIn < 20) {
    visitorCount++;

    lcd.setCursor(0, 0);
    lcd.print(F("Visitors: "));
    lcd.print(visitorCount);
    lcd.print(F("  "));
    delay(2000);
  } else if (distOut > 0 && distOut < 20 && visitorCount > 0) {
    visitorCount--;

    lcd.setCursor(0, 0);
    lcd.print(F("Visitors: "));
    lcd.print(visitorCount);
    lcd.print(F("  "));
    delay(2000);
  }
}

void motorOpen() {
  analogWrite(MOTOR_R, 255);
  analogWrite(MOTOR_L, 0);
  lcd.setCursor(0, 1);
  lcd.print("Gate Open       ");
  delay(500);
  motorStop();
}

void motorClose() {
  analogWrite(MOTOR_R, 0);
  analogWrite(MOTOR_L, 255);
  lcd.setCursor(0, 1);
  lcd.print("Gate Closed     ");
  delay(500);
  motorStop();
}

void motorStop() {
  analogWrite(MOTOR_R, 0);
  analogWrite(MOTOR_L, 0);
}

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
