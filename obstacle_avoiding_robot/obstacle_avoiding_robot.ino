/*
  2-wheel obstacle avoiding robot

  Hardware assumed:
  - Arduino Uno/Nano compatible board
  - L298N / L293D style dual H-bridge motor driver
  - HC-SR04 ultrasonic distance sensor
  - Relay module

  Behavior:
  - Drives forward while the path is clear.
  - Turns the relay ON only while moving forward.
  - Stops, reverses briefly, then turns when an obstacle is too close.
*/

// Motor driver pins
const byte ENA = 2;   // Left motor enable, must support PWM
const byte IN1 = 3;
const byte IN2 = 6;

const byte ENB = 5;   // Right motor enable, must support PWM
const byte IN3 = 7;
const byte IN4 = 8;

// Ultrasonic sensor pins
const byte TRIG_PIN = 12;
const byte ECHO_PIN = 11;

// Relay pin
const byte RELAY_PIN = 9;

// Tune these for your robot
const int FORWARD_SPEED = 180;          // 0-255
const int TURN_SPEED = 175;             // 0-255
const int REVERSE_SPEED = 165;          // 0-255
const int OBSTACLE_DISTANCE_CM = 20;    // Turn when obstacle is closer than this

const unsigned long REVERSE_TIME_MS = 300;
const unsigned long TURN_TIME_MS = 450;
const unsigned long SENSOR_TIMEOUT_US = 25000; // About 4 meters max range

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  stopMotors();
}

void loop() {
  int distanceCm = readDistanceCm();

  if (distanceCm > 0 && distanceCm <= OBSTACLE_DISTANCE_CM) {
    avoidObstacle();
  } else {
    moveForward();
  }

  delay(50);
}

int readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

    // Read Echo
  unsigned long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate distance in cm
  return duration * 0.0343 / 2.0;
}

void avoidObstacle() {
  stopMotors();
  delay(120);

  moveBackward();
  delay(REVERSE_TIME_MS);

  turnRight();
  delay(TURN_TIME_MS);

  stopMotors();
}

void moveForward() {
  digitalWrite(RELAY_PIN, HIGH);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, FORWARD_SPEED);
  analogWrite(ENB, FORWARD_SPEED);
}

void moveBackward() {
  digitalWrite(RELAY_PIN, LOW);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, REVERSE_SPEED);
  analogWrite(ENB, REVERSE_SPEED);
}

void turnRight() {
  digitalWrite(RELAY_PIN, LOW);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
}

void stopMotors() {
  digitalWrite(RELAY_PIN, LOW);

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
