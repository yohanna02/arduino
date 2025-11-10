// --- Motor Driver Pins ---
#define LEFT_MOTOR_FWD 3
#define LEFT_MOTOR_BWD 4
#define RIGHT_MOTOR_FWD 5
#define RIGHT_MOTOR_BWD 6

#define ENA 9   // Left motor enable (PWM speed control)
#define ENB 10  // Right motor enable (PWM speed control)

#define RELAY_PIN 2  // Relay control pin

char command;
int speedValue = 145;     // Default speed (0-255)
bool relayState = false;  // Relay toggle state

void setup() {
  // Motor pins
  pinMode(LEFT_MOTOR_FWD, OUTPUT);
  pinMode(LEFT_MOTOR_BWD, OUTPUT);
  pinMode(RIGHT_MOTOR_FWD, OUTPUT);
  pinMode(RIGHT_MOTOR_BWD, OUTPUT);

  // Enable pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  Serial.begin(9600);  // Bluetooth module baud rate
}

void loop() {
  if (Serial.available() > 0) {
    command = Serial.read();

    switch (command) {
      case 'F': forward(); break;   // Forward
      case 'B': backward(); break;  // Backward
      case 'L': left(); break;      // Left
      case 'R': right(); break;     // Right
      case 'S':
        stopCar();
        break;  // Stop

      // Speed control (4 levels)
      case '1': speedValue = 80; break;   // Low speed
      case '2': speedValue = 150; break;  // Medium speed
      case '3': speedValue = 200; break;  // Fast
      case '4':
        speedValue = 255;
        break;  // Max speed

      // Relay toggle
      case 'X':
        relayState = !relayState;
        digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
        digitalWrite(13, relayState ? HIGH : LOW);
        break;
    }
  }
}

// --- Motor Control Functions ---
void stopCar() {
  digitalWrite(LEFT_MOTOR_FWD, LOW);
  digitalWrite(LEFT_MOTOR_BWD, LOW);
  digitalWrite(RIGHT_MOTOR_FWD, LOW);
  digitalWrite(RIGHT_MOTOR_BWD, LOW);
}

void forward() {
  digitalWrite(LEFT_MOTOR_FWD, HIGH);
  digitalWrite(LEFT_MOTOR_BWD, LOW);
  digitalWrite(RIGHT_MOTOR_FWD, LOW);
  digitalWrite(RIGHT_MOTOR_BWD, HIGH);
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void backward() {
  digitalWrite(LEFT_MOTOR_FWD, LOW);
  digitalWrite(LEFT_MOTOR_BWD, HIGH);
  digitalWrite(RIGHT_MOTOR_FWD, HIGH);
  digitalWrite(RIGHT_MOTOR_BWD, LOW);
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void left() {
  digitalWrite(LEFT_MOTOR_FWD, HIGH);
  digitalWrite(LEFT_MOTOR_BWD, LOW);
  digitalWrite(RIGHT_MOTOR_FWD, HIGH);
  digitalWrite(RIGHT_MOTOR_BWD, LOW);
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void right() {
  digitalWrite(LEFT_MOTOR_FWD, LOW);
  digitalWrite(LEFT_MOTOR_BWD, HIGH);
  digitalWrite(RIGHT_MOTOR_FWD, LOW);
  digitalWrite(RIGHT_MOTOR_BWD, HIGH);
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}