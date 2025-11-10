// --- Pin Definitions ---
#define TRIG_FRONT 12
#define ECHO_FRONT A5

#define TRIG_LEFT_FRONT 5
#define ECHO_LEFT_FRONT 4

#define TRIG_LEFT_BACK 2
#define ECHO_LEFT_BACK 3

#define LEFT_MOTOR_FWD 7
#define LEFT_MOTOR_BWD 8
#define RIGHT_MOTOR_FWD 9
#define RIGHT_MOTOR_BWD 10

// #define LEFT_MOTOR_PWM 5   // PWM pin for left motor enable
// #define RIGHT_MOTOR_PWM 6  // PWM pin for right motor enable

// Arm and Wrist Motors (DC)
#define ARM_UP_PIN A0      // control arm up direction
#define ARM_DOWN_PIN A1    // control arm down direction
#define ARM_PWM 11         // PWM enable pin for arm speed
#define ARM_POT_PIN A4

// PID or P control
const float ARM_KP = 0.7;
const int ARM_DEADZONE = 10;
const int ARM_MAX_PWM = 220;

#define WRIST_LEFT_PIN A2  // rotate wrist left
#define WRIST_RIGHT_PIN A3 // rotate wrist right
// #define WRIST_PWM 27       // PWM enable pin for wrist speed

#define MODE_SWITCH 6     // Switch input pin

// --- Constants ---
#define OBSTACLE_DISTANCE 20   // cm
#define WALL_DISTANCE 15       // cm desired distance from wall
#define BASE_SPEED 150         // wheel motor base speed
#define TURN_SPEED 120         // turning speed
#define ARM_SPEED 200          // PWM for arm motor
#define WRIST_SPEED 200        // PWM for wrist motor

// --- Function Prototypes ---
float getDistance(int trigPin, int echoPin);
void moveForward(int leftSpeed, int rightSpeed);
void stopMotors();
void turnRight();
void turnLeft();
void groundSweepMode();
void wallSweepMode();
void armDown();
void armUp();
void wristCenter();
void wristFaceWall();

bool lastMode = false; // track mode changes

// --- Arm control variables ---
const int ARM_MIN = 100;   // Pot reading for arm fully down (measure & adjust)
const int ARM_MAX = 900;   // Pot reading for arm fully up (measure & adjust)
int armTarget = 0;

void setup() {
  Serial.begin(9600);

  // Sensor pins
  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);
  pinMode(TRIG_LEFT_FRONT, OUTPUT);
  pinMode(ECHO_LEFT_FRONT, INPUT);
  pinMode(TRIG_LEFT_BACK, OUTPUT);
  pinMode(ECHO_LEFT_BACK, INPUT);

  // Drive motors
  pinMode(LEFT_MOTOR_FWD, OUTPUT);
  pinMode(LEFT_MOTOR_BWD, OUTPUT);
  pinMode(RIGHT_MOTOR_FWD, OUTPUT);
  pinMode(RIGHT_MOTOR_BWD, OUTPUT);

  // Arm motors
  pinMode(ARM_UP_PIN, OUTPUT);
  pinMode(ARM_DOWN_PIN, OUTPUT);
  pinMode(ARM_PWM, OUTPUT);

  // Wrist motors
  pinMode(WRIST_LEFT_PIN, OUTPUT);
  pinMode(WRIST_RIGHT_PIN, OUTPUT);

  pinMode(MODE_SWITCH, INPUT_PULLUP);

  Serial.println("Robotic Vacuum Cleaner Ready");
}

void loop() {
  armUp();
  delay(3000);
  armDown();
  delay(3000);
  // bool wallMode = digitalRead(MODE_SWITCH) == HIGH;

  // // If mode changed, adjust arm/wrist once
  // if (wallMode != lastMode) {
  //   stopMotors();
  //   if (wallMode) {
  //     Serial.println("Switching to WALL mode...");
  //     armUp();
  //     wristFaceWall();
  //   } else {
  //     Serial.println("Switching to GROUND mode...");
  //     armDown();
  //     wristCenter();
  //   }
  //   lastMode = wallMode;
  // }

  // // Mode logic
  // if (wallMode) {
  //   wallSweepMode();
  // } else {
  //   groundSweepMode();
  // }
}

// --- Distance Measurement ---
float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  float distance = duration * 0.0343 / 2;
  return distance;
}

// --- Drive Motor Controls ---
void moveForward(int leftSpeed, int rightSpeed) {
  digitalWrite(LEFT_MOTOR_FWD, HIGH);
  digitalWrite(LEFT_MOTOR_BWD, LOW);
  digitalWrite(RIGHT_MOTOR_FWD, HIGH);
  digitalWrite(RIGHT_MOTOR_BWD, LOW);
}

void stopMotors() {
  digitalWrite(LEFT_MOTOR_FWD, LOW);
  digitalWrite(LEFT_MOTOR_BWD, LOW);
  digitalWrite(RIGHT_MOTOR_FWD, LOW);
  digitalWrite(RIGHT_MOTOR_BWD, LOW);
}

void turnRight() {
  digitalWrite(LEFT_MOTOR_FWD, HIGH);
  digitalWrite(LEFT_MOTOR_BWD, LOW);
  digitalWrite(RIGHT_MOTOR_FWD, LOW);
  digitalWrite(RIGHT_MOTOR_BWD, HIGH);
}

void turnLeft() {
  digitalWrite(LEFT_MOTOR_FWD, LOW);
  digitalWrite(LEFT_MOTOR_BWD, HIGH);
  digitalWrite(RIGHT_MOTOR_FWD, HIGH);
  digitalWrite(RIGHT_MOTOR_BWD, LOW);
}

// --- Arm and Wrist Controls ---
void moveArmToTarget(int targetADC) {
  int current = analogRead(ARM_POT_PIN);
  int error = targetADC - current;

  while (abs(error) > ARM_DEADZONE) {
    current = analogRead(ARM_POT_PIN);
    error = targetADC - current;

    int pwm = constrain(abs(error) * ARM_KP, 80, ARM_MAX_PWM);

    if (error > 0) {
      // Move arm up
      digitalWrite(ARM_DOWN_PIN, LOW);
      digitalWrite(ARM_UP_PIN, HIGH);
    } else {
      // Move arm down
      digitalWrite(ARM_UP_PIN, LOW);
      digitalWrite(ARM_DOWN_PIN, HIGH);
    }

    analogWrite(ARM_PWM, pwm);
    delay(10);
  }

  // Stop arm when target reached
  digitalWrite(ARM_UP_PIN, LOW);
  digitalWrite(ARM_DOWN_PIN, LOW);
  analogWrite(ARM_PWM, 0);
}

void armDown() {
  Serial.println("Arm Down (Scrubbing)");
  moveArmToTarget(ARM_MIN);
}

void armUp() {
  Serial.println("Arm Up (Wall Mode)");
  moveArmToTarget(ARM_MAX);
}

void wristCenter() {
  Serial.println("Wrist Centered");
  digitalWrite(WRIST_LEFT_PIN, LOW);
  digitalWrite(WRIST_RIGHT_PIN, LOW);
}

void wristFaceWall() {
  Serial.println("Wrist Facing Wall");
  digitalWrite(WRIST_LEFT_PIN, HIGH);
  digitalWrite(WRIST_RIGHT_PIN, LOW);
  delay(500); // adjust for rotation time
  digitalWrite(WRIST_LEFT_PIN, LOW);
}

// --- Ground Sweep Mode ---
void groundSweepMode() {
  float frontDist = getDistance(TRIG_FRONT, ECHO_FRONT);

  if (frontDist < OBSTACLE_DISTANCE) {
    stopMotors();
    delay(200);
    turnRight();
    delay(500);
    stopMotors();
  } else {
    moveForward(BASE_SPEED, BASE_SPEED);
  }
}

// --- Wall Sweep Mode ---
void wallSweepMode() {
  float leftFront = getDistance(TRIG_LEFT_FRONT, ECHO_LEFT_FRONT);
  float leftBack = getDistance(TRIG_LEFT_BACK, ECHO_LEFT_BACK);
  float frontDist = getDistance(TRIG_FRONT, ECHO_FRONT);

  if (frontDist < OBSTACLE_DISTANCE) {
    stopMotors();
    delay(200);
    turnRight();
    delay(500);
    stopMotors();
    return;
  }

  float avgDist = (leftFront + leftBack) / 2.0;
  float alignmentError = leftFront - leftBack;
  float distanceError = WALL_DISTANCE - avgDist;

  float correction = (distanceError * 8) + (alignmentError * 6);
  correction = constrain(correction, -80, 80);

  int leftSpeed = BASE_SPEED - correction;
  int rightSpeed = BASE_SPEED + correction;

  moveForward(leftSpeed, rightSpeed);

  Serial.print("LF: "); Serial.print(leftFront);
  Serial.print(" | LB: "); Serial.print(leftBack);
  Serial.print(" | F: "); Serial.print(frontDist);
  Serial.print(" | Corr: "); Serial.println(correction);
}
