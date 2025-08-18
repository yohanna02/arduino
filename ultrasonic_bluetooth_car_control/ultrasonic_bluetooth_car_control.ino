#define trigFront 2
#define echoFront 12
#define trigBack 6
#define echoBack 7

#define motor1A 5
#define motor1B 4

#define RPWM 11
#define LPWM 10
#define R_EN 8
#define L_EN 9

#define pump1 13

char command;
#define bluetooth Serial

void setup() {
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);

  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);

  pinMode(pump1, OUTPUT);

  pinMode(trigFront, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(trigBack, OUTPUT);
  pinMode(echoBack, INPUT);

  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);

  bluetooth.begin(9600);
}

void loop() {
  if (bluetooth.available()) {
    command = bluetooth.read();
    handleCommand(command);
  }

  long frontDist = readDistance(trigFront, echoFront);
  long backDist = readDistance(trigBack, echoBack);

  if (frontDist <= 10 && frontDist >= 4 && command == 'F') {
    stopMotors();
  } else if (backDist <= 10 && backDist >= 4 && command == 'B') {
    stopMotors();
  }
}

void handleCommand(char cmd) {
  switch (cmd) {
    case 'F':
      {
        long frontDist = readDistance(trigFront, echoFront);
        if (frontDist > 10 || frontDist < 4)  // safe if outside that range
          moveForward();
        else
          stopMotors();
        break;
      }
    case 'B':
      {
        long backDist = readDistance(trigBack, echoBack);
        if (backDist > 10 || backDist < 4)
          moveBackward();
        else
          stopMotors();
        break;
      }
    case 'L':
      turnLeft();
      break;

    case 'R':
      turnRight();
      break;

    case 'S':
      stopMotors();
      break;

    case 'I':
      digitalWrite(pump1, HIGH);
      break;

    case 'M':
      digitalWrite(pump1, LOW);
      break;

    default:
      stopMotors();
      // digitalWrite(pump1, LOW);
  }
}

void moveForward() {
  analogWrite(RPWM, 200);  // Forward speed
  analogWrite(LPWM, 0);
}

void moveBackward() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 200);
}

void turnLeft() {
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, HIGH);
}

void turnRight() {
  digitalWrite(motor1A, HIGH);
  digitalWrite(motor1B, LOW);
}

void stopMotors() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, LOW);
}

long readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}
