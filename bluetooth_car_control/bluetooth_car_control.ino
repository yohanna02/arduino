#include <SoftwareSerial.h>

SoftwareSerial esp_serial(2, 3);

#define _serial esp_serial

// Motor pins
int motorA1 = 8;
int motorA2 = 7;
int motorB1 = 6;
int motorB2 = 5;

int ENA = 10;
int ENB = 9;

// Ultrasonic pins
int trigPin = 11;
int echoPin = 12;

// Variables
char command;
char lastCommand = 'S'; // 'S' for stop
long duration;
int distance;
const int safeDistance = 20; // in cm

void setup() {
  _serial.begin(9600);

  // Motor pins
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Ultrasonic pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Stop();
}

void loop() {
  // Read new command if available
  if (_serial.available() > 0) {
    command = _serial.read();
    lastCommand = command;
  }

  // Always check distance
  distance = getDistance();

  // If moving forward and obstacle appears
  if (lastCommand == 'F' && distance <= safeDistance) {
    Serial.println("Obstacle detected! Turning right...");
    turnRightFor(500); // Turn right for 500ms
    lastCommand = 'F'; // Continue moving forward after turning
  } else {
    // Continue executing last command
    executeCommand(lastCommand);
  }
}

// Execute stored command
void executeCommand(char cmd) {
  switch (cmd) {
    case 'F':
      forward();
      break;
    case 'B':
      back();
      break;
    case 'L':
      left();
      break;
    case 'R':
      right();
      break;
    default:
      Stop();
      break;
  }
}

// Ultrasonic distance measurement
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 20000); // 20ms timeout
  if (duration == 0) return 999; // No object detected

  return duration * 0.034 / 2; // Convert to cm
}

// Motor functions
void forward() {
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);
  digitalWrite(motorA1, HIGH);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, HIGH);
  digitalWrite(motorB2, LOW);
}

void back() {
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, HIGH);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, HIGH);
}

void left() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 200);
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, HIGH);
  digitalWrite(motorB2, LOW);
}

void right() {
  analogWrite(ENA, 200);
  analogWrite(ENB, 0);
  digitalWrite(motorA1, HIGH);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, LOW);
}

void Stop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, LOW);
}

// Turn right for a set time
void turnRightFor(int timeMs) {
  right();
  delay(timeMs);
  forward();
}
