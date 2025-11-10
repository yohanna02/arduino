// --- Pin Definitions ---
// Ultrasonic Sensor
#define TRIG_PIN 12
#define ECHO_PIN 11

// Motor Driver (H-bridge direct pins)
#define LEFT_MOTOR_FWD 7
#define LEFT_MOTOR_BWD 8
#define RIGHT_MOTOR_FWD 9
#define RIGHT_MOTOR_BWD 10

long duration;
int distance;

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LEFT_MOTOR_FWD, OUTPUT);
  pinMode(LEFT_MOTOR_BWD, OUTPUT);
  pinMode(RIGHT_MOTOR_FWD, OUTPUT);
  pinMode(RIGHT_MOTOR_BWD, OUTPUT);

  stopCar();
}

void loop() {
  // moveForward();
  distance = getDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 10 && distance <= 30) {
    // Move forward if object is detected between 10cm and 50cm
    moveForward();
  }
  else if (distance <= 10 && distance > 0) {
    // Too close -> stop
    stopCar();
  } 
  else {
    // No object in range (>50cm or invalid)
    stopCar();
  }

  delay(100);
}

// --- Functions ---

int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 20000); // timeout 20ms
  if (duration == 0) return -1; // no echo
  return duration * 0.034 / 2; // convert to cm
}

void moveForward() {
  digitalWrite(LEFT_MOTOR_FWD, HIGH);
  digitalWrite(LEFT_MOTOR_BWD, LOW);
  digitalWrite(RIGHT_MOTOR_FWD, HIGH);
  digitalWrite(RIGHT_MOTOR_BWD, LOW);
}

void stopCar() {
  digitalWrite(LEFT_MOTOR_FWD, LOW);
  digitalWrite(LEFT_MOTOR_BWD, LOW);
  digitalWrite(RIGHT_MOTOR_FWD, LOW);
  digitalWrite(RIGHT_MOTOR_BWD, LOW);
}
