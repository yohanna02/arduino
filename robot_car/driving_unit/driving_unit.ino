#include <ESPComm.h>
#include <SoftwareSerial.h>

// Define pins for front ultrasonic sensor
#define FRONT_TRIG 5
#define FRONT_ECHO 6

// Define pins for back ultrasonic sensor
#define BACK_TRIG 3
#define BACK_ECHO 4

// Distance threshold (in cm)
#define DISTANCE_THRESHOLD 25

#define RPWM_1 10
#define LPWM_1 9

#define TURN_A 7
#define TURN_B 8

SoftwareSerial _serial(12, 13);

#define bluetooth _serial

ESPComm drivingUnit(Serial);

enum class DrivingMode {
  NONE,
  BLUETOOTH,
  AUTO
};

DrivingMode currentDrivingMode = DrivingMode::NONE;
char command;

void moveForward(int angle = 90);

void onCommandReceived(String key, String value) {
  if (key == "MODE") {
    if (value == "AUTO") {
      currentDrivingMode = DrivingMode::AUTO;
      drivingUnit.send("MODE", "AUTO");
    } else if (value == "BLUETOOTH") {
      currentDrivingMode = DrivingMode::BLUETOOTH;
      drivingUnit.send("MODE", "BLUETOOTH");
    } else if (value == "NONE") {
      currentDrivingMode = DrivingMode::NONE;
      drivingUnit.send("MODE", "NONE");
    }
    stopMotors();
    command = 0;
  }
}

void setup() {
  pinMode(RPWM_1, OUTPUT);
  pinMode(LPWM_1, OUTPUT);
  pinMode(TURN_A, OUTPUT);
  pinMode(TURN_B, OUTPUT);

  pinMode(FRONT_ECHO, INPUT);
  pinMode(FRONT_TRIG, OUTPUT);

  pinMode(BACK_ECHO, INPUT);
  pinMode(BACK_TRIG, OUTPUT);

  bluetooth.begin(9600);
  drivingUnit.begin(9600);
  drivingUnit.onCommand(onCommandReceived);
}

void loop() {
  // moveForward();
  // delay(5000);
  // moveBackward();
  // delay(3000);
  drivingUnit.loop();

  float frontDistance = readUltrasonic(FRONT_TRIG, FRONT_ECHO);
  float backDistance = readUltrasonic(BACK_TRIG, BACK_ECHO);

  if (currentDrivingMode == DrivingMode::AUTO) {

    // Obstacle avoidance logic
    if (frontDistance < DISTANCE_THRESHOLD) {
      stopMotors();
      delay(200);
      if (backDistance > DISTANCE_THRESHOLD) {
        moveBackward();
        delay(5000);
        turnRight();
        delay(500);
      } else {
        turnLeft();
        delay(500);
      }
    } else if (backDistance < DISTANCE_THRESHOLD) {
      stopMotors();
      delay(200);
      moveForward();
      delay(500);
    } else {
      moveForward();
    }
  } else if (currentDrivingMode == DrivingMode::BLUETOOTH) {
    if (bluetooth.available()) {
      command = bluetooth.read();
      if (command == 'F') {
        drivingUnit.send("DRIVE", "F");
      } else if (command == 'B') {
        drivingUnit.send("DRIVE", "B");
      } else if (command == 'S') {
        drivingUnit.send("DRIVE", "S");
      }
    }

    if (command == 'F') {
      if (frontDistance < DISTANCE_THRESHOLD) {
        stopMotors();
        command = 0;
        return;
      }
      moveForward();
    } else if (command == 'B') {
      if (backDistance < DISTANCE_THRESHOLD) {
        stopMotors();
        command = 0;
        return;
      }
      moveBackward();
    } else if (command == 'L') {
      turnLeft();
    } else if (command == 'R') {
      turnRight();
    } else {
      stopMotors();
    }
  }
}

float readUltrasonic(int trigPin, int echoPin) {
  // Send pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo time
  long duration = pulseIn(echoPin, HIGH);

  // Calculate distance in cm
  float distance = duration * 0.0343 / 2;

  return distance;
}

// Motor control functions
void moveForward(int angle = 90) {
  analogWrite(RPWM_1, 255);
  analogWrite(LPWM_1, 0);
}

void moveBackward() {
  analogWrite(RPWM_1, 0);
  analogWrite(LPWM_1, 255);
}

void turnLeft() {
  digitalWrite(TURN_A, HIGH);
  digitalWrite(TURN_B, LOW);
}

void turnRight() {
  digitalWrite(TURN_A, LOW);
  digitalWrite(TURN_B, HIGH);
}

void stopMotors() {
  analogWrite(RPWM_1, 0);
  analogWrite(LPWM_1, 0);

  digitalWrite(TURN_A, LOW);
  digitalWrite(TURN_B, LOW);
}