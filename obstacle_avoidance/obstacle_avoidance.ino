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

// #define RELAY_3 5
// #define RELAY_4 A0

// #define R_S 12  //ir sensor Right
// #define L_S A5   //ir sensor Left

#define PI_PIN 7
#define PUMP 5

#define CAMERA_ROTATE_1 2
#define CAMERA_ROTATE_2 4
#define CAMERA_ROTATE_SPEED 3

unsigned long moveMillis = 0;
bool toggleMove = true;

#define CAMERA_ROTATE_DURATION 3500  // Rotation time (adjust as needed)
#define CAMERA_WAIT_TIME 2000        // Wait time after rotating

int stopState = 0;
unsigned long cameraRotateMillis = 0;
bool cameraRotating = false;
bool rotateClockwise = true;  // Start with clockwise rotation
bool scanning = false;
int scanStep = 0;  // Tracks scanning steps
unsigned long cameraMillis = 0;

bool auto_mode = true;

#define AUTO_MODE "a_m:"
#define MANUAL_MODE "m_m:"

#define FORWARD "f:"
#define BACKWARD "b:"
#define STOP "s:"

#define ON_PUMP "p_o:"
#define OFF_PUMP "p_f:"

void setup() {
  // put your setup code here, to run once:
  // pinMode(R_S, INPUT);
  // pinMode(L_S, INPUT);

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
  // pinMode(RELAY_3, OUTPUT);
  // digitalWrite(RELAY_3, HIGH);
  // pinMode(RELAY_4, OUTPUT);

  pinMode(CAMERA_ROTATE_1, OUTPUT);
  pinMode(CAMERA_ROTATE_2, OUTPUT);
  pinMode(CAMERA_ROTATE_SPEED, OUTPUT);

  delay(60000);

  // pinMode(8, OUTPUT);
  // pinMode(7, OUTPUT);

  // digitalWrite(8, HIGH);
  // digitalWrite(7, HIGH);

  // while (1) {
  //   digitalWrite(RELAY_1, HIGH);
  //   digitalWrite(RELAY_2, LOW);

  //   digitalWrite(RELAY_3, LOW);
  //   digitalWrite(RELAY_4, HIGH);
  //   delay(10000);

  //   Stop();
  //   delay(2000);

  //   digitalWrite(RELAY_1, LOW);
  //   digitalWrite(RELAY_2, HIGH);

  //   digitalWrite(RELAY_3, HIGH);
  //   digitalWrite(RELAY_4, LOW);
  //   delay(10000);

  //   Stop();
  //   delay(2000);
  // }
}

void loop() {
  // put your main code here, to run repeatedly:
  int distance_one = getDistance(TRIG_ONE, ECHO_ONE);
  int distance_two = getDistance(TRIG_TWO, ECHO_TWO);

  if ((distance_one > 3 && distance_one < 10) || (distance_two > 3 && distance_two < 10)) {
    Stop();
    digitalWrite(PUMP, LOW);
    while ((distance_one > 5 && distance_one < 13) || (distance_two > 5 && distance_two < 13)) {
      distance_one = getDistance(TRIG_ONE, ECHO_ONE);
      distance_two = getDistance(TRIG_TWO, ECHO_TWO);
    }
    delay(3000);
  }

  if (digitalRead(PI_PIN)) {
    // if (getDistance(TANK_TRIG_TWO, TANK_TRIG_TWO) < ) {
    // }
    digitalWrite(PUMP, HIGH);
    delay(2000);
    digitalWrite(PUMP, LOW);
    delay(5000);
    toggleMove = true;
  }

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
    } else if (command == OFF_PUMP && !auto_mode) {
      digitalWrite(PUMP, LOW);
    }
  }

  if (auto_mode) {
    if (!scanning) {
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
      cameraMillis = millis();  // Start scan timer
    }

    if (scanning) {
      unsigned long elapsed = millis() - cameraMillis;

      if (scanStep == 0 && elapsed < CAMERA_ROTATE_DURATION) {
        rotateCameraClockwise();
      } else if (scanStep == 0 && elapsed >= CAMERA_ROTATE_DURATION) {
        stopCameraRotation();
        scanStep = 1;
        cameraMillis = millis();  // Start wait timer
      } else if (scanStep == 1 && elapsed >= CAMERA_WAIT_TIME) {
        scanStep = 2;
        cameraMillis = millis();  // Start second rotation timer
      } else if (scanStep == 2 && elapsed < CAMERA_ROTATE_DURATION) {
        rotateCameraCounterclockwise();
      } else if (scanStep == 2 && elapsed >= CAMERA_ROTATE_DURATION) {
        stopCameraRotation();
        scanStep = 3;
        cameraMillis = millis();  // Start second wait timer
      } else if (scanStep == 3 && elapsed >= CAMERA_WAIT_TIME) {
        scanning = false;  // Resume movement
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

  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a 10-microsecond pulse to trigPin
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin and calculate the distance
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;  // Convert time to distance (cm)

  return distance;  // Return the distance value
}

void forward() {  //forword
  analogWrite(RELAY_1, 255);
  analogWrite(RELAY_2, 0);
  // digitalWrite(RELAY_1, HIGH);
  // digitalWrite(RELAY_2, LOW);

  // digitalWrite(RELAY_3, HIGH);
  // digitalWrite(RELAY_4, LOW);
}

void backward() {  //forword
  analogWrite(RELAY_1, 0);
  analogWrite(RELAY_2, 255);
  // digitalWrite(RELAY_1, LOW);
  // digitalWrite(RELAY_2, HIGH);

  // digitalWrite(RELAY_3, LOW);
  // digitalWrite(RELAY_4, HIGH);
}

// void turnRight() {  //turnRight
//   digitalWrite(RELAY_1, HIGH);
//   digitalWrite(RELAY_2, LOW);

//   digitalWrite(RELAY_3, LOW);
//   digitalWrite(RELAY_4, LOW);
// }

// void turnLeft() {  //turnLeft
//   digitalWrite(RELAY_1, LOW);
//   digitalWrite(RELAY_2, LOW);

//   digitalWrite(RELAY_3, HIGH);
//   digitalWrite(RELAY_4, LOW);
// }

void Stop() {  //stop
  analogWrite(RELAY_1, 0);
  analogWrite(RELAY_2, 0);
  // digitalWrite(RELAY_1, LOW);
  // digitalWrite(RELAY_2, LOW);

  // digitalWrite(RELAY_3, LOW);
  // digitalWrite(RELAY_4, LOW);
}

/*   // Serial.println(F("Moving"));
  if ((digitalRead(R_S) == 0) && (digitalRead(L_S) == 0)) {
    forward(); //if Right Sensor and Left Sensor are at White color then it will call forword function
  }
  if ((digitalRead(R_S) == 1) && (digitalRead(L_S) == 0)) {
    turnRight(); //if Right Sensor is Black and Left Sensor is White then it will call turn Right function
  }
  if ((digitalRead(R_S) == 0) && (digitalRead(L_S) == 1)) {
    turnLeft(); //if Right Sensor is White and Left Sensor is Black then it will call turn Left function
  }
  if ((digitalRead(R_S) == 1) && (digitalRead(L_S) == 1)) {
    Stop(); //if Right Sensor and Left Sensor are at Black color then it will call Stop function
  } */
