// Define pins for front ultrasonic sensor
#define FRONT_TRIG 7
#define FRONT_ECHO 6

// Define pins for back ultrasonic sensor
#define BACK_TRIG 5
#define BACK_ECHO 4

// Define motor driver pins
#define LEFT_MOTOR_FORWARD 8
#define LEFT_MOTOR_BACKWARD 9
#define RIGHT_MOTOR_FORWARD 10
#define RIGHT_MOTOR_BACKWARD 11

// Distance threshold (in cm)
#define DISTANCE_THRESHOLD 20

void setup() {
  // Initialize pins
  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);
  pinMode(BACK_TRIG, OUTPUT);
  pinMode(BACK_ECHO, INPUT);

  pinMode(LEFT_MOTOR_FORWARD, OUTPUT);
  pinMode(LEFT_MOTOR_BACKWARD, OUTPUT);
  pinMode(RIGHT_MOTOR_FORWARD, OUTPUT);
  pinMode(RIGHT_MOTOR_BACKWARD, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // Measure distances
  long frontDistance = measureDistance(FRONT_TRIG, FRONT_ECHO);
  long backDistance = measureDistance(BACK_TRIG, BACK_ECHO);

  Serial.print("Front Distance: ");
  Serial.println(frontDistance);
  Serial.print("Back Distance: ");
  Serial.println(backDistance);

  // Obstacle avoidance logic
  if (frontDistance < DISTANCE_THRESHOLD) {
    stopMotors();
    delay(200);
    if (backDistance > DISTANCE_THRESHOLD) {
      moveBackward();
      delay(500);
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

  delay(100);
}

// Function to measure distance from an ultrasonic sensor
long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // Convert to cm
  return distance;
}

// Motor control functions
void moveForward() {
  digitalWrite(LEFT_MOTOR_FORWARD, HIGH);
  digitalWrite(LEFT_MOTOR_BACKWARD, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD, HIGH);
  digitalWrite(RIGHT_MOTOR_BACKWARD, LOW);
}

void moveBackward() {
  digitalWrite(LEFT_MOTOR_FORWARD, LOW);
  digitalWrite(LEFT_MOTOR_BACKWARD, HIGH);
  digitalWrite(RIGHT_MOTOR_FORWARD, LOW);
  digitalWrite(RIGHT_MOTOR_BACKWARD, HIGH);
}

void turnLeft() {
  digitalWrite(LEFT_MOTOR_FORWARD, LOW);
  digitalWrite(LEFT_MOTOR_BACKWARD, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD, HIGH);
  digitalWrite(RIGHT_MOTOR_BACKWARD, LOW);
}

void turnRight() {
  digitalWrite(LEFT_MOTOR_FORWARD, HIGH);
  digitalWrite(LEFT_MOTOR_BACKWARD, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD, LOW);
  digitalWrite(RIGHT_MOTOR_BACKWARD, LOW);
}

void stopMotors() {
  digitalWrite(LEFT_MOTOR_FORWARD, LOW);
  digitalWrite(LEFT_MOTOR_BACKWARD, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD, LOW);
  digitalWrite(RIGHT_MOTOR_BACKWARD, LOW);
}
