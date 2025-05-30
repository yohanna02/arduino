// Motor Pins
const int IN1 = 8;
const int IN2 = 7;
const int IN3 = A3;
const int IN4 = A4;
const int ENA = 9;
const int ENB = 10;

// Ultrasonic Pins
const int trigFront = 6;
const int echoFront = 5;
const int trigBack = 3;
const int echoBack = 4;

// LED Pins
const int ledAuto = 12;
const int ledManual = 11;

// Mode
bool autoMode = false;

void setup() {
  // Motor Pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Ultrasonic Pins
  pinMode(trigFront, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(trigBack, OUTPUT);
  pinMode(echoBack, INPUT);

  // LED Pins
  pinMode(ledAuto, OUTPUT);
  pinMode(ledManual, OUTPUT);

  Serial.begin(9600);
  updateLEDs();
}

void loop() {
  // Check Bluetooth input
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == '1') {
      autoMode = true;
      updateLEDs();
    } else if (cmd == '2') {
      autoMode = false;
      updateLEDs();
      stopMotors();  // Immediately stop when switching to manual
    } else if (!autoMode) {
      manualControl(cmd);
    }
  }

  if (autoMode) {
    int frontDistance = getDistance(trigFront, echoFront);
    int backDistance = getDistance(trigBack, echoBack);

    if (frontDistance < 20) {
      stopMotors();
      delay(200);
      moveBackward();
      delay(500);  // Move back a bit
      stopMotors();
      delay(200);
      turnLeft();  // or use turnRight() if preferred
      delay(600);  // Turn for a moment
      stopMotors();
      delay(200);
    } else if (backDistance < 20) {
      stopMotors();  // Avoid backing into something
      delay(200);
    } else {
      moveForward();
    }
  }
}

void updateLEDs() {
  digitalWrite(ledAuto, autoMode ? HIGH : LOW);
  digitalWrite(ledManual, autoMode ? LOW : HIGH);
}

int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);  // Timeout after 30ms
  int distance = duration * 0.034 / 2;
  return distance;
}

// Motor Control
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 120);
  analogWrite(ENB, 120);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 120);
  analogWrite(ENB, 120);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 120);
  analogWrite(ENB, 120);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 120);
  analogWrite(ENB, 120);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// Manual Control via Bluetooth
void manualControl(char cmd) {
  switch (cmd) {
    case 'F': moveForward(); break;
    case 'B': moveBackward(); break;
    case 'L': turnLeft(); break;
    case 'R': turnRight(); break;
    case 'S': stopMotors(); break;
  }
}
