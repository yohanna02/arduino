const int R_EN = A2;
const int L_EN = A3;

const int R_PWM = 6;
const int L_PWM = 5;

// Ultrasonic sensors
const int trigFront = 12;
const int echoFront = 11;
const int trigRear = 9;
const int echoRear = 10;
const int trigTank = 8;
const int echoTank = 7;  // shifted to 7

// Pump and light control
const int pumpPin = 2;
const int lightPin = 4;

// Raspberry Pi pump control pin
const int piPumpControlPin = 3;

// Variables
char command;
// bool bluetoothPumpRequest = false;
bool piPumpRequest = false;
bool lightState = false;

void setup() {
  Serial.begin(9600);

  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);

  pinMode(trigFront, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(trigRear, OUTPUT);
  pinMode(echoRear, INPUT);
  pinMode(trigTank, OUTPUT);
  pinMode(echoTank, INPUT);

  pinMode(pumpPin, OUTPUT);
  pinMode(lightPin, OUTPUT);

  // pinMode(piPumpControlPin, INPUT);

  stopMotors();
  digitalWrite(pumpPin, LOW);
  digitalWrite(lightPin, LOW);
}

void loop() {
  if (Serial.available()) {
    command = Serial.read();
    handleCommand(command);
  }

  // Always check Pi signal
  // piPumpRequest = digitalRead(piPumpControlPin) == HIGH;

  // Check water level
  // int waterLevel = measureDistance(trigTank, echoTank);

  // Only turn on pump if there is enough water
  // if ((bluetoothPumpRequest || piPumpRequest) && waterLevel <= 10) {  // Example: 10cm threshold
  //   digitalWrite(pumpPin, HIGH);
  // } else {
  //   digitalWrite(pumpPin, LOW);
  // }
}

void handleCommand(char cmd) {
  switch (cmd) {
    case 'F':
      moveForward();
      break;
    case 'B':
      moveBackward();
      break;
    case 'L':
      turnLeft();
      break;
    case 'R':
      turnRight();
      break;
    case 'S':
      stopMotors();
      break;
    case 'A':
      {
        int waterLevel = measureDistance(trigTank, echoTank);

        if (waterLevel <= 10) {
          digitalWrite(pumpPin, HIGH);
          delay(1500);
          digitalWrite(pumpPin, LOW);
        }
        break;
      }
    case 'E':
      digitalWrite(pumpPin, LOW);
      break;
    case 'C':
      digitalWrite(lightPin, HIGH);
      break;
    case 'D':
      digitalWrite(lightPin, LOW);
      break;
  }
}

void moveForward() {
  long frontDist = measureDistance(trigFront, echoFront);
  if (frontDist > 20 || frontDist == 0) {  // no obstacle or very far
    digitalWrite(R_EN, HIGH);
    digitalWrite(L_EN, HIGH);
    analogWrite(R_PWM, 0);
    analogWrite(L_PWM, 255);
  } else {
    stopMotors();
  }
}

void moveBackward() {
  long rearDist = measureDistance(trigRear, echoRear);
  if (rearDist > 20 || rearDist == 0) {
    digitalWrite(R_EN, HIGH);
    digitalWrite(L_EN, HIGH);
    analogWrite(R_PWM, 255);
    analogWrite(L_PWM, 0);
  } else {
    stopMotors();
  }
}

void turnLeft() {
  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);
  analogWrite(R_PWM, 255);
  analogWrite(L_PWM, 0);
}

void turnRight() {
  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);
  analogWrite(R_PWM, 0);
  analogWrite(L_PWM, 255);
}

void stopMotors() {
  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);
  analogWrite(R_PWM, 0);
  analogWrite(L_PWM, 0);
}

int measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);  // 30ms timeout to avoid hanging
  int distance = duration * 0.034 / 2;            // Distance in cm
  return distance;
}
