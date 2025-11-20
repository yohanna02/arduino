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
#define MOTOR_R 12
#define MOTOR_L 10
#define MOTOR_EN 11

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
  pinMode(MOTOR_EN, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("Construction"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("of a microcontroller"));
  lcd.setCursor(0, 1);
  lcd.print(F("based gate control"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("system with a"));
  lcd.setCursor(0, 1);
  lcd.print(F("digital counter"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("BONHEUR,"));
  lcd.setCursor(0, 1);
  lcd.print(F("ALLASRBAYE"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("23/151059"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("MAL.YAQOUB ISAH"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("ALJASAWI"));
  delay(3000);
  lcd.clear();
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
  digitalWrite(MOTOR_R, HIGH);
  digitalWrite(MOTOR_L, LOW);
  analogWrite(MOTOR_EN, 255);
  lcd.setCursor(0, 1);
  lcd.print("Gate Open       ");
  delay(200);
  motorStop();
}

void motorClose() {
  digitalWrite(MOTOR_R, LOW);
  digitalWrite(MOTOR_L, HIGH);
  analogWrite(MOTOR_EN, 255);
  lcd.setCursor(0, 1);
  lcd.print("Gate Closed     ");
  delay(200);
  motorStop();
}

void motorStop() {
  digitalWrite(MOTOR_R, LOW);
  digitalWrite(MOTOR_L, LOW);
  analogWrite(MOTOR_EN, 0);
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
