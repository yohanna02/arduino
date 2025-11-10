#include <Servo.h>

// === PIN ASSIGNMENTS ===
// Ultrasonic
const int trigPin = 12;
const int echoPin = 11;

// IR Sensor
const int irPin = 3;

// Car Motor Driver
const int carEna = 2;
const int carIn1 = A5;
const int carIn2 = A6;

// Arm Motor Driver 1 (Base + Elbow)
const int armEna1 = 10;
const int armIn1a = 8;
const int armIn2a = 9;
const int potBase = A0;
const int potElbow = A1;

// Arm Motor Driver 2 (Wrist + Gripper Servo)
const int armEna2 = 7;
const int armIn1b = 6;
const int armIn2b = 5;
const int potWrist = A2;

// Servos
Servo wristServo;
Servo gripperServo;
const int wristPin = A3;
const int gripperPin = A4;

// === VARIABLES ===
long duration;
int distance;

// === SETUP ===
void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(irPin, INPUT);

  pinMode(carEna, OUTPUT);
  pinMode(carIn1, OUTPUT);
  pinMode(carIn2, OUTPUT);

  pinMode(armEna1, OUTPUT);
  pinMode(armIn1a, OUTPUT);
  pinMode(armIn2a, OUTPUT);

  pinMode(armEna2, OUTPUT);
  pinMode(armIn1b, OUTPUT);
  pinMode(armIn2b, OUTPUT);

  wristServo.attach(wristPin);
  gripperServo.attach(gripperPin);

  Serial.println("System initializing... Please wait 30 seconds.");
  delay(30000);  // 30-second startup delay

  Serial.println("System Ready. Starting line following and scanning...");
}

// === FUNCTIONS ===

// Ultrasonic distance measure
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  int dist = duration * 0.034 / 2;
  return dist;
}

// Car control
void carForward() {
  digitalWrite(carIn1, HIGH);
  digitalWrite(carIn2, LOW);
  analogWrite(carEna, 150);
}
void carStop() {
  digitalWrite(carIn1, LOW);
  digitalWrite(carIn2, LOW);
  analogWrite(carEna, 0);
}

// Arm movement (simplified simulation)
void pickTomato() {
  Serial.println("Tomato detected! Starting picking sequence...");
  
  // Move base
  analogWrite(armEna1, 180);
  digitalWrite(armIn1a, HIGH);
  digitalWrite(armIn2a, LOW);
  delay(1000);

  // Move elbow forward
  analogWrite(armEna1, 200);
  digitalWrite(armIn1a, LOW);
  digitalWrite(armIn2a, HIGH);
  delay(1000);

  // Lower wrist
  analogWrite(armEna2, 150);
  digitalWrite(armIn1b, HIGH);
  digitalWrite(armIn2b, LOW);
  delay(1000);
  analogWrite(armEna2, 0);

  // Open gripper
  gripperServo.write(90);
  delay(1000);
  // Close gripper
  gripperServo.write(10);
  delay(1000);

  // Lift wrist back up
  analogWrite(armEna2, 150);
  digitalWrite(armIn1b, LOW);
  digitalWrite(armIn2b, HIGH);
  delay(1000);
  analogWrite(armEna2, 0);

  // Drop tomato into basket
  gripperServo.write(90);
  delay(1000);

  Serial.println("Tomato picked and placed in basket!");
}

// === MAIN LOOP ===
void loop() {
  int irState = digitalRead(irPin);
  int distance = getDistance();

  if (irState == LOW) { // Following black line
    carForward();
  } else {
    carStop();
  }

  if (distance < 15) {  // Object detected
    carStop();
    pickTomato();
    delay(2000);
  }
}
