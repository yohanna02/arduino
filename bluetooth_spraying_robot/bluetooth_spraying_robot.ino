// ===== ULTRASONIC SENSOR =====
#define WATER_TRIG 4
#define WATER_ECHO A0

#define FRONT_TRIG A1
#define FRONT_ECHO A2

#define BACK_TRIG 2
#define BACK_ECHO 12

#define MAX_DISTANCE_CM 17  // adjust based on tank depth

#define OBSTACLE_DISTANCE_CM 20

// ===== SHARED ENABLE PINS =====
#define EN_R 8
#define EN_L 7

// ===== LEFT DRIVE MOTOR =====
#define L_RPWM 5
#define L_LPWM 3

// ===== RIGHT DRIVE MOTOR =====
#define R_RPWM 9
#define R_LPWM 10

// ===== DIRECTION MOTOR =====
#define D_RPWM 6
#define D_LPWM 11

// ===== PUMP =====
#define PUMP_PIN 13  // Relay or MOSFET

#define CAMERA_ACTION_PIN A5

#define SPEED 255  // 0–255

char cmd;
bool pumpState = false;

void setup() {
  Serial.begin(9600);  // HC-05 default baud

  pinMode(WATER_TRIG, OUTPUT);
  pinMode(WATER_ECHO, INPUT);

  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(BACK_TRIG, OUTPUT);
  pinMode(BACK_ECHO, INPUT);

  // Enable pins
  pinMode(EN_R, OUTPUT);
  pinMode(EN_L, OUTPUT);

  // Motors
  pinMode(L_RPWM, OUTPUT);
  pinMode(L_LPWM, OUTPUT);

  pinMode(R_RPWM, OUTPUT);
  pinMode(R_LPWM, OUTPUT);

  pinMode(D_RPWM, OUTPUT);
  pinMode(D_LPWM, OUTPUT);

  // Pump
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(CAMERA_ACTION_PIN, INPUT);

  // Enable all BTS7960 drivers
  digitalWrite(EN_R, HIGH);
  digitalWrite(EN_L, HIGH);

  stopAll();
  pumpOff();
}

void loop() {
  if (Serial.available()) {
    cmd = Serial.read();
  }

  switch (cmd) {
    case 'F':
      if (!isFrontBlocked()) {
        moveForward();
      } else {
        stopAll();
      }
      break;

    case 'B':
      if (!isBackBlocked()) {
        moveBackward();
      } else {
        stopAll();
      }
      break;

    case 'L':
      turnLeft();
      break;

    case 'R':
      turnRight();
      break;

    case 'S':
      stopAll();
      break;

    case 'O':
      if (isWaterAvailable()) {
        pumpOn();
      }
      break;

    case 'P':
      pumpOff();
      break;

    default:
      stopAll();
      break;
  }

  // Emergency stop while moving
  if (cmd == 'F' && isFrontBlocked()) {
    stopAll();
    cmd = 'S';
  }

  if (cmd == 'B' && isBackBlocked()) {
    stopAll();
    cmd = 'S';
  }

  // Auto-protect pump while running
  if (pumpState && !isWaterAvailable()) {
    pumpOff();
  }

  if (!pumpState && isWaterAvailable() && digitalRead(CAMERA_ACTION_PIN) == HIGH) {
    delay(200);
    if (digitalRead(CAMERA_ACTION_PIN) == LOW) {
      return;
    }

    stopAll();
    delay(200);
    pumpOn();
    delay(1000);
    pumpOff();
  }
}

// ===== MOVEMENT FUNCTIONS =====

void moveForward() {
  analogWrite(L_RPWM, 0);
  analogWrite(L_LPWM, SPEED);

  analogWrite(R_RPWM, 0);
  analogWrite(R_LPWM, SPEED);
}

void moveBackward() {
  analogWrite(L_RPWM, SPEED);
  analogWrite(L_LPWM, 0);

  analogWrite(R_RPWM, SPEED);
  analogWrite(R_LPWM, 0);
}

void turnLeft() {
  analogWrite(D_RPWM, 0);
  analogWrite(D_LPWM, SPEED);
}

void turnRight() {
  analogWrite(D_RPWM, SPEED);
  analogWrite(D_LPWM, 0);
}

void stopAll() {
  analogWrite(L_RPWM, 0);
  analogWrite(L_LPWM, 0);

  analogWrite(R_RPWM, 0);
  analogWrite(R_LPWM, 0);

  analogWrite(D_RPWM, 0);
  analogWrite(D_LPWM, 0);
}

// ===== PUMP =====

void pumpOn() {
  digitalWrite(PUMP_PIN, LOW);
  pumpState = true;
}

void pumpOff() {
  digitalWrite(PUMP_PIN, HIGH);
  pumpState = false;
}

// ===== ULTRASONIC FUNCTIONS =====

bool isWaterAvailable() {
  long duration;
  int distance;

  digitalWrite(WATER_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(WATER_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(WATER_TRIG, LOW);

  duration = pulseIn(WATER_ECHO, HIGH, 30000);  // timeout 30ms

  distance = duration * 0.034 / 2;

  if (distance == 0) return true;

  return distance <= MAX_DISTANCE_CM;
}

int getDistance(int trigPin, int echoPin) {
  long duration;
  int distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);  // 30ms timeout

  if (duration == 0) return 999;  // No reading = assume far

  distance = duration * 0.034 / 2;
  return distance;
}

bool isBackBlocked() {
  int distance = getDistance(BACK_TRIG, BACK_ECHO);
  return distance <= OBSTACLE_DISTANCE_CM;
}

bool isFrontBlocked() {
  int distance = getDistance(FRONT_TRIG, FRONT_ECHO);
  return distance <= OBSTACLE_DISTANCE_CM;
}