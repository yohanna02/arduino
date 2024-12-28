// Define motor driver pins
#define ENA 6   // Enable pin for Motor A
#define IN1 A0   // Control pin 1 for Motor A
#define IN2 A1   // Control pin 2 for Motor A
#define ENB 5  // Enable pin for Motor B
#define IN3 A2   // Control pin 1 for Motor B
#define IN4 A3   // Control pin 2 for Motor B

// Bluetooth communication
#define RX 2  // Bluetooth module RX pin
#define TX 3  // Bluetooth module TX pin

#include <SoftwareSerial.h>
SoftwareSerial Bluetooth(RX, TX);

#define BLUETOOTH_SERIAL Bluetooth

char command;

void setup() {
  // Set motor driver pins as output
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Initialize Bluetooth communication
  BLUETOOTH_SERIAL.begin(9600);

  // Set default motor speeds
  analogWrite(ENA, 120);  // Full speed for Motor A
  analogWrite(ENB, 120);  // Full speed for Motor B

  Serial.begin(9600);  // Debugging on serial monitor
  Serial.println("Bluetooth Car Robot Ready");
}

void loop() {
  if (BLUETOOTH_SERIAL.available()) {
    command = BLUETOOTH_SERIAL.read();  // Read the command from the Bluetooth app
    Serial.println(command);          // Print the command for debugging
  }

  switch (command) {
    case 'F':  // Forward
      moveForward();
      break;
    case 'B':  // Backward
      moveBackward();
      break;
    case 'L':  // Left
      turnLeft();
      break;
    case 'R':  // Right
      turnRight();
      break;
    case 'S':  // Stop
      stopMotors();
      break;
    default:
      stopMotors();  // Stop if invalid command
      break;
  }
}

void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
