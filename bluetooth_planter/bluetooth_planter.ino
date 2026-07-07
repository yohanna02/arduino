// ===== SHARED ENABLE PINS =====
#define EN 7

// ===== LEFT DRIVE MOTOR =====
#define RPWM 9
#define LPWM 10

// ===== Plant https://github.com/arduino/arduino-ideSHARED ENABLE PINS =====
#define SHAFT_EN 8

// ===== LEFT DRIVE MOTOR =====
#define SHAFT_RPWM 3
#define SHAFT_LPWM 5

#define SPEED 255  // 0 - 255
#define SHAFT_SPEED 255  // 0 - 255

#define FRONT_TRIG A1
#define FRONT_ECHO A2

#define BACK_TRIG A3
#define BACK_ECHO A4

#define OBSTACLE_DISTANCE_CM 20

char cmd = 0;

unsigned long prev_start_millis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(BACK_TRIG, OUTPUT);
  pinMode(BACK_ECHO, INPUT);

  // Motors
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(SHAFT_RPWM, OUTPUT);
  pinMode(SHAFT_LPWM, OUTPUT);
  pinMode(SHAFT_EN, OUTPUT);

  // Enable all BTS7960 drivers
  digitalWrite(EN, HIGH);
  digitalWrite(SHAFT_EN, HIGH);

  stopAll();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    cmd = Serial.read();

    if (cmd == 'F') {
      prev_start_millis = millis();
    }
  }

  switch (cmd) {
    case 'F':
      moveForward();
      break;

    case 'B':
      moveBackward();
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

  if (millis() - prev_start_millis > 2000 && cmd == 'F') {
    shaftMove();
  }
}
// ===== SHARED ENABLE PINS =====
#define EN 7

// ===== LEFT DRIVE MOTOR =====
#define RPWM 9
#define LPWM 10

// ===== Plant https://github.com/arduino/arduino-ideSHARED ENABLE PINS =====
#define SHAFT_EN 8

// ===== LEFT DRIVE MOTOR =====
#define SHAFT_RPWM 3
#define SHAFT_LPWM 5

#define SPEED 255  // 0 - 255
#define SHAFT_SPEED 255  // 0 - 255

#define FRONT_TRIG A1
#define FRONT_ECHO A2

#define BACK_TRIG A3
#define BACK_ECHO A4

#define OBSTACLE_DISTANCE_CM 20

char cmd = 0;

unsigned long prev_start_millis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(BACK_TRIG, OUTPUT);
  pinMode(BACK_ECHO, INPUT);

  // Motors
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(SHAFT_RPWM, OUTPUT);
  pinMode(SHAFT_LPWM, OUTPUT);
  pinMode(SHAFT_EN, OUTPUT);

  // Enable all BTS7960 drivers
  digitalWrite(EN, HIGH);
  digitalWrite(SHAFT_EN, HIGH);

  stopAll();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    cmd = Serial.read();

    if (cmd == 'F') {
      prev_start_millis = millis();
    }
  }

  switch (cmd) {
    case 'F':
      moveForward();
      break;

    case 'B':
      moveBackward();
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

  if (millis() - prev_start_millis > 2000 && cmd == 'F') {
    shaftMove();
  }
}

void moveForward() {
  analogWrite(RPWM, SPEED);
  analogWrite(LPWM, 0);
}

void moveBackward() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, SPEED);
}

void stopAll() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);

  analogWrite(SHAFT_RPWM, 0);
  analogWrite(SHAFT_LPWM, 0);
}

void shaftMove() {
  analogWrite(SHAFT_RPWM, SHAFT_SPEED);
  analogWrite(SHAFT_LPWM, 0);
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
}// ===== SHARED ENABLE PINS =====
#define EN 7

// ===== LEFT DRIVE MOTOR =====
#define RPWM 9
#define LPWM 10

// ===== Plant https://github.com/arduino/arduino-ideSHARED ENABLE PINS =====
#define SHAFT_EN 8

// ===== LEFT DRIVE MOTOR =====
#define SHAFT_RPWM 3
#define SHAFT_LPWM 5

#define SPEED 255  // 0 - 255
#define SHAFT_SPEED 255  // 0 - 255

#define FRONT_TRIG A1
#define FRONT_ECHO A2

#define BACK_TRIG A3
#define BACK_ECHO A4

#define OBSTACLE_DISTANCE_CM 20

char cmd = 0;

unsigned long prev_start_millis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(BACK_TRIG, OUTPUT);
  pinMode(BACK_ECHO, INPUT);

  // Motors
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(SHAFT_RPWM, OUTPUT);
  pinMode(SHAFT_LPWM, OUTPUT);
  pinMode(SHAFT_EN, OUTPUT);

  // Enable all BTS7960 drivers
  digitalWrite(EN, HIGH);
  digitalWrite(SHAFT_EN, HIGH);

  stopAll();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    cmd = Serial.read();

    if (cmd == 'F') {
      prev_start_millis = millis();
    }
  }

  switch (cmd) {
    case 'F':
      moveForward();
      break;

    case 'B':
      moveBackward();
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

  if (millis() - prev_start_millis > 2000 && cmd == 'F') {
    shaftMove();
  }
}

void moveForward() {
  analogWrite(RPWM, SPEED);
  analogWrite(LPWM, 0);
}

void moveBackward() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, SPEED);
}

void stopAll() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);

  analogWrite(SHAFT_RPWM, 0);
  analogWrite(SHAFT_LPWM, 0);
}

void shaftMove() {
  analogWrite(SHAFT_RPWM, SHAFT_SPEED);
  analogWrite(SHAFT_LPWM, 0);
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
}// ===== SHARED ENABLE PINS =====
#define EN 7

// ===== LEFT DRIVE MOTOR =====
#define RPWM 9
#define LPWM 10

// ===== Plant https://github.com/arduino/arduino-ideSHARED ENABLE PINS =====
#define SHAFT_EN 8

// ===== LEFT DRIVE MOTOR =====
#define SHAFT_RPWM 3
#define SHAFT_LPWM 5

#define SPEED 255  // 0 - 255
#define SHAFT_SPEED 255  // 0 - 255

#define FRONT_TRIG A1
#define FRONT_ECHO A2

#define BACK_TRIG A3
#define BACK_ECHO A4

#define OBSTACLE_DISTANCE_CM 20

char cmd = 0;

unsigned long prev_start_millis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(BACK_TRIG, OUTPUT);
  pinMode(BACK_ECHO, INPUT);

  // Motors
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(SHAFT_RPWM, OUTPUT);
  pinMode(SHAFT_LPWM, OUTPUT);
  pinMode(SHAFT_EN, OUTPUT);

  // Enable all BTS7960 drivers
  digitalWrite(EN, HIGH);
  digitalWrite(SHAFT_EN, HIGH);

  stopAll();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    cmd = Serial.read();

    if (cmd == 'F') {
      prev_start_millis = millis();
    }
  }

  switch (cmd) {
    case 'F':
      moveForward();
      break;

    case 'B':
      moveBackward();
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

  if (millis() - prev_start_millis > 2000 && cmd == 'F') {
    shaftMove();
  }
}

void moveForward() {
  analogWrite(RPWM, SPEED);
  analogWrite(LPWM, 0);
}

void moveBackward() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, SPEED);
}

void stopAll() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);

  analogWrite(SHAFT_RPWM, 0);
  analogWrite(SHAFT_LPWM, 0);
}

void shaftMove() {
  analogWrite(SHAFT_RPWM, SHAFT_SPEED);
  analogWrite(SHAFT_LPWM, 0);
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
}// ===== SHARED ENABLE PINS =====
#define EN 7

// ===== LEFT DRIVE MOTOR =====
#define RPWM 9
#define LPWM 10

// ===== Plant https://github.com/arduino/arduino-ideSHARED ENABLE PINS =====
#define SHAFT_EN 8

// ===== LEFT DRIVE MOTOR =====
#define SHAFT_RPWM 3
#define SHAFT_LPWM 5

#define SPEED 255  // 0 - 255
#define SHAFT_SPEED 255  // 0 - 255

#define FRONT_TRIG A1
#define FRONT_ECHO A2

#define BACK_TRIG A3
#define BACK_ECHO A4

#define OBSTACLE_DISTANCE_CM 20

char cmd = 0;

unsigned long prev_start_millis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(BACK_TRIG, OUTPUT);
  pinMode(BACK_ECHO, INPUT);

  // Motors
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(SHAFT_RPWM, OUTPUT);
  pinMode(SHAFT_LPWM, OUTPUT);
  pinMode(SHAFT_EN, OUTPUT);

  // Enable all BTS7960 drivers
  digitalWrite(EN, HIGH);
  digitalWrite(SHAFT_EN, HIGH);

  stopAll();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    cmd = Serial.read();

    if (cmd == 'F') {
      prev_start_millis = millis();
    }
  }

  switch (cmd) {
    case 'F':
      moveForward();
      break;

    case 'B':
      moveBackward();
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

  if (millis() - prev_start_millis > 2000 && cmd == 'F') {
    shaftMove();
  }
}

void moveForward() {
  analogWrite(RPWM, SPEED);
  analogWrite(LPWM, 0);
}

void moveBackward() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, SPEED);
}

void stopAll() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);

  analogWrite(SHAFT_RPWM, 0);
  analogWrite(SHAFT_LPWM, 0);
}

void shaftMove() {
  analogWrite(SHAFT_RPWM, SHAFT_SPEED);
  analogWrite(SHAFT_LPWM, 0);
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
}// ===== SHARED ENABLE PINS =====
#define EN 7

// ===== LEFT DRIVE MOTOR =====
#define RPWM 9
#define LPWM 10

// ===== Plant https://github.com/arduino/arduino-ideSHARED ENABLE PINS =====
#define SHAFT_EN 8

// ===== LEFT DRIVE MOTOR =====
#define SHAFT_RPWM 3
#define SHAFT_LPWM 5

#define SPEED 255  // 0 - 255
#define SHAFT_SPEED 255  // 0 - 255

#define FRONT_TRIG A1
#define FRONT_ECHO A2

#define BACK_TRIG A3
#define BACK_ECHO A4

#define OBSTACLE_DISTANCE_CM 20

char cmd = 0;

unsigned long prev_start_millis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(BACK_TRIG, OUTPUT);
  pinMode(BACK_ECHO, INPUT);

  // Motors
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(SHAFT_RPWM, OUTPUT);
  pinMode(SHAFT_LPWM, OUTPUT);
  pinMode(SHAFT_EN, OUTPUT);

  // Enable all BTS7960 drivers
  digitalWrite(EN, HIGH);
  digitalWrite(SHAFT_EN, HIGH);

  stopAll();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    cmd = Serial.read();

    if (cmd == 'F') {
      prev_start_millis = millis();
    }
  }

  switch (cmd) {
    case 'F':
      moveForward();
      break;

    case 'B':
      moveBackward();
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

  if (millis() - prev_start_millis > 2000 && cmd == 'F') {
    shaftMove();
  }
}

void moveForward() {
  analogWrite(RPWM, SPEED);
  analogWrite(LPWM, 0);
}

void moveBackward() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, SPEED);
}

void stopAll() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);

  analogWrite(SHAFT_RPWM, 0);
  analogWrite(SHAFT_LPWM, 0);
}

void shaftMove() {
  analogWrite(SHAFT_RPWM, SHAFT_SPEED);
  analogWrite(SHAFT_LPWM, 0);
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
}// ===== SHARED ENABLE PINS =====
#define EN 7

// ===== LEFT DRIVE MOTOR =====
#define RPWM 9
#define LPWM 10

// ===== Plant https://github.com/arduino/arduino-ideSHARED ENABLE PINS =====
#define SHAFT_EN 8

// ===== LEFT DRIVE MOTOR =====
#define SHAFT_RPWM 3
#define SHAFT_LPWM 5

#define SPEED 255  // 0 - 255
#define SHAFT_SPEED 255  // 0 - 255

#define FRONT_TRIG A1
#define FRONT_ECHO A2

#define BACK_TRIG A3
#define BACK_ECHO A4

#define OBSTACLE_DISTANCE_CM 20

char cmd = 0;

unsigned long prev_start_millis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(BACK_TRIG, OUTPUT);
  pinMode(BACK_ECHO, INPUT);

  // Motors
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(SHAFT_RPWM, OUTPUT);
  pinMode(SHAFT_LPWM, OUTPUT);
  pinMode(SHAFT_EN, OUTPUT);

  // Enable all BTS7960 drivers
  digitalWrite(EN, HIGH);
  digitalWrite(SHAFT_EN, HIGH);

  stopAll();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    cmd = Serial.read();

    if (cmd == 'F') {
      prev_start_millis = millis();
    }
  }

  switch (cmd) {
    case 'F':
      moveForward();
      break;

    case 'B':
      moveBackward();
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

  if (millis() - prev_start_millis > 2000 && cmd == 'F') {
    shaftMove();
  }
}

void moveForward() {
  analogWrite(RPWM, SPEED);
  analogWrite(LPWM, 0);
}

void moveBackward() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, SPEED);
}

void stopAll() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);

  analogWrite(SHAFT_RPWM, 0);
  analogWrite(SHAFT_LPWM, 0);
}stopAll// ===== SHARED ENABLE PINS =====
#define EN 7

// ===== LEFT DRIVE MOTOR =====
#define RPWM 9
#define LPWM 10

// ===== Plant https://github.com/arduino/arduino-ideSHARED ENABLE PINS =====
#define SHAFT_EN 8

// ===== LEFT DRIVE MOTOR =====
#define SHAFT_RPWM 3
#define SHAFT_LPWM 5

#define SPEED 255  // 0 - 255
#define SHAFT_SPEED 255  // 0 - 255

#define FRONT_TRIG A1
#define FRONT_ECHO A2

#define BACK_TRIG A3
#define BACK_ECHO A4

#define OBSTACLE_DISTANCE_CM 20

char cmd = 0;

unsigned long prev_start_millis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(BACK_TRIG, OUTPUT);
  pinMode(BACK_ECHO, INPUT);

  // Motors
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(SHAFT_RPWM, OUTPUT);
  pinMode(SHAFT_LPWM, OUTPUT);
  pinMode(SHAFT_EN, OUTPUT);

  // Enable all BTS7960 drivers
  digitalWrite(EN, HIGH);
  digitalWrite(SHAFT_EN, HIGH);

  stopAll();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    cmd = Serial.read();

    if (cmd == 'F') {
      prev_start_millis = millis();
    }
  }

  switch (cmd) {
    case 'F':
      moveForward();
      break;

    case 'B':
      moveBackward();
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

  if (millis() - prev_start_millis > 2000 && cmd == 'F') {
    shaftMove();
  }
}

void moveForward() {
  analogWrite(RPWM, SPEED);
  analogWrite(LPWM, 0);
}

void moveBackward() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, SPEED);
}

void stopAll() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);

  analogWrite(SHAFT_RPWM, 0);
  analogWrite(SHAFT_LPWM, 0);
}

void shaftMove() {
  analogWrite(SHAFT_RPWM, SHAFT_SPEED);
  analogWrite(SHAFT_LPWM, 0);
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

void shaftMove() {
  analogWrite(SHAFT_RPWM, SHAFT_SPEED);
  analogWrite(SHAFT_LPWM, 0);
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
}stopAll

bool isBackBlocked() {
  int distance = getDistance(BACK_TRIG, BACK_ECHO);
  return distance <= OBSTACLE_DISTANCE_CM;
}

bool isFrontBlocked() {
  int distance = getDistance(FRONT_TRIG, FRONT_ECHO);
  return distance <= OBSTACLE_DISTANCE_CM;
}
void moveForward() {
  analogWrite(RPWM, SPEED);
  analogWrite(LPWM, 0);
}

void moveBackward() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, SPEED);
}

void stopAll() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);

  analogWrite(SHAFT_RPWM, 0);
  analogWrite(SHAFT_LPWM, 0);
}

void shaftMove() {
  analogWrite(SHAFT_RPWM, SHAFT_SPEED);
  analogWrite(SHAFT_LPWM, 0);
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