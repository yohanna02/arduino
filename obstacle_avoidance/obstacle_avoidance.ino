#include <SoftwareSerial.h>

SoftwareSerial _serial(12, 13);

#define ESP_SERIAL _serial

#define TRIG_ONE 11
#define ECHO_ONE 10

#define TRIG_TWO A2
#define ECHO_TWO A1

#define TANK_TRIG_TWO A3
#define TANK_ECHO_TWO A4

#define RELAY_1 9
#define RELAY_2 6

#define PI_PIN 7
#define PUMP 5

#define CAMERA_ROTATE_1 2
#define CAMERA_ROTATE_2 4
#define CAMERA_ROTATE_SPEED 3

unsigned long moveMillis = 0;
bool toggleMove = true;

#define CAMERA_ROTATE_DURATION 3500
#define CAMERA_WAIT_TIME 2000

int stopState = 0;
unsigned long cameraRotateMillis = 0;
bool cameraRotating = false;
bool rotateClockwise = true;
bool scanning = false;
int scanStep = 0;
unsigned long cameraMillis = 0;

bool auto_mode = true;
bool pumpRunning = false;   // ✅ Added pump state

#define AUTO_MODE "a_m:"
#define MANUAL_MODE "m_m:"

#define FORWARD "f:"
#define BACKWARD "b:"
#define STOP "s:"

#define ON_PUMP "p_o:"
#define OFF_PUMP "p_f:"

void setup() {

  ESP_SERIAL.begin(9600);

  pinMode(PI_PIN, INPUT);
  pinMode(PUMP, OUTPUT);

  pinMode(TRIG_ONE, OUTPUT);
  pinMode(ECHO_ONE, INPUT);
  pinMode(TRIG_TWO, OUTPUT);
  pinMode(ECHO_TWO, INPUT);
  pinMode(TANK_TRIG_TWO, OUTPUT);
  pinMode(TANK_ECHO_TWO, INPUT);

  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);

  pinMode(CAMERA_ROTATE_1, OUTPUT);
  pinMode(CAMERA_ROTATE_2, OUTPUT);
  pinMode(CAMERA_ROTATE_SPEED, OUTPUT);

  delay(60000);
}

void loop() {

  int distance_one = getDistance(TRIG_ONE, ECHO_ONE);
  int distance_two = getDistance(TRIG_TWO, ECHO_TWO);

  if ((distance_one > 3 && distance_one < 15) || (distance_two > 3 && distance_two < 15)) {
    Stop();
    digitalWrite(PUMP, LOW);
    pumpRunning = false;

    while ((distance_one > 5 && distance_one < 15) || (distance_two > 5 && distance_two < 15)) {
      distance_one = getDistance(TRIG_ONE, ECHO_ONE);
      distance_two = getDistance(TRIG_TWO, ECHO_TWO);
    }
    delay(3000);
  }

  // Pump Trigger from PI
  if (digitalRead(PI_PIN)) {

    digitalWrite(PUMP, HIGH);
    pumpRunning = true;
    scanning = false;
    stopCameraRotation();  // stop camera immediately

    delay(2000);

    digitalWrite(PUMP, LOW);
    pumpRunning = false;

    delay(5000);
    toggleMove = true;
  }

  // Serial Commands
  if (ESP_SERIAL.available()) {
    String command = ESP_SERIAL.readStringUntil(':') + ":";

    if (command == AUTO_MODE) {
      auto_mode = true;
      ESP_SERIAL.print(AUTO_MODE);

    } else if (command == MANUAL_MODE) {
      auto_mode = false;
      ESP_SERIAL.print(MANUAL_MODE);

    } else if (command == FORWARD) {
      forward();

    } else if (command == BACKWARD) {
      backward();

    } else if (command == STOP) {
      Stop();

    } else if (command == ON_PUMP && !auto_mode) {

      digitalWrite(PUMP, HIGH);
      pumpRunning = true;
      scanning = false;
      stopCameraRotation();

    } else if (command == OFF_PUMP && !auto_mode) {

      digitalWrite(PUMP, LOW);
      pumpRunning = false;
    }
  }

  // AUTO MODE
  if (auto_mode) {

    if (!scanning && !pumpRunning) {

      forward();
      delay(3000);

      Stop();
      delay(1000);

      distance_one = getDistance(TRIG_ONE, ECHO_ONE);
      distance_two = getDistance(TRIG_TWO, ECHO_TWO);

      if (distance_one < 15 || distance_two < 15) {

        backward();
        delay(5000);
        Stop();
      }

      scanning = true;
      scanStep = 0;
      cameraMillis = millis();
    }

    if (scanning && !pumpRunning) {

      unsigned long elapsed = millis() - cameraMillis;

      if (scanStep == 0 && elapsed < CAMERA_ROTATE_DURATION) {

        rotateCameraClockwise();

      } else if (scanStep == 0 && elapsed >= CAMERA_ROTATE_DURATION) {

        stopCameraRotation();
        scanStep = 1;
        cameraMillis = millis();

      } else if (scanStep == 1 && elapsed >= CAMERA_WAIT_TIME) {

        scanStep = 2;
        cameraMillis = millis();

      } else if (scanStep == 2 && elapsed < CAMERA_ROTATE_DURATION) {

        rotateCameraCounterclockwise();

      } else if (scanStep == 2 && elapsed >= CAMERA_ROTATE_DURATION) {

        stopCameraRotation();
        scanStep = 3;
        cameraMillis = millis();

      } else if (scanStep == 3 && elapsed >= CAMERA_WAIT_TIME) {

        scanning = false;
      }
    }

  } else {

    stopCameraRotation();
  }
}

void rotateCameraClockwise() {

  digitalWrite(CAMERA_ROTATE_1, HIGH);
  digitalWrite(CAMERA_ROTATE_2, LOW);
  analogWrite(CAMERA_ROTATE_SPEED, 255);
}

void rotateCameraCounterclockwise() {

  digitalWrite(CAMERA_ROTATE_1, LOW);
  digitalWrite(CAMERA_ROTATE_2, HIGH);
  analogWrite(CAMERA_ROTATE_SPEED, 255);
}

void stopCameraRotation() {

  digitalWrite(CAMERA_ROTATE_1, LOW);
  digitalWrite(CAMERA_ROTATE_2, LOW);
  analogWrite(CAMERA_ROTATE_SPEED, 0);
}

int getDistance(int trigPin, int echoPin) {

  long duration;
  int distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  return distance;
}

void forward() {

  analogWrite(RELAY_1, 255);
  analogWrite(RELAY_2, 0);
}

void backward() {

  analogWrite(RELAY_1, 0);
  analogWrite(RELAY_2, 255);
}

void Stop() {

  analogWrite(RELAY_1, 0);
  analogWrite(RELAY_2, 0);
}