
#include<Servo.h>
//DefinethepinsfortheDCmotor
const int motorForwardPin = 2;
const int motorBackwardPin = 3;
//Definethepinsfortheservomotor
const int servoPin = 9;
//Definethepinsforthelightandhorn
const int lightPin = 8;
const int hornPin = 12;
//Createaservoobject
 Servo servo;
//DefinetheBluetoothmodule'sserialcommunicationpins
const int bluetoothTx = 10;
const int bluetoothRx = 11;
//Initializevariablestotrackmovement
int movementSteps = 0;
char movementDirection;
void setup() {
  //InitializetheserialcommunicationfortheBluetoothmodule
  Serial.begin(9600);
  //InitializethepinsfortheDCmotor
  pinMode(motorForwardPin, OUTPUT);
  pinMode(motorBackwardPin, OUTPUT);
  //Initializethepinsfortheservomotor
  servo.attach(servoPin);
  //Initializethepinsforthelightandhorn
  pinMode(lightPin, OUTPUT);
  pinMode(hornPin, OUTPUT);
}
  void loop() {
  char command;
  
    //ReadtheincomingBluetoothdata
    if (Serial.available() > 0) {
      char command = Serial.read();
      //Forwardmovement
      if (command == 'F') {
        digitalWrite(motorForwardPin, HIGH);
        digitalWrite(motorBackwardPin, LOW);
        servo.write(90);
        movementSteps++;
        movementDirection = 'F';
      }
      //Backwardmovement
      else if(command == 'B') {
        digitalWrite(motorForwardPin, LOW);
        digitalWrite(motorBackwardPin, HIGH);
        servo.write(90);
        movementSteps++;
        movementDirection = 'B';
      }
      //Forwardleftmovement
      else if(command == 'G') {
        digitalWrite(motorForwardPin, HIGH);
        digitalWrite(motorBackwardPin, LOW);
        servo.write(45);
        movementSteps++;
        movementDirection = 'G';
      }
      //Forwardrightmovement
      else if(command == 'I') {
        digitalWrite(motorForwardPin, HIGH);
        digitalWrite(motorBackwardPin, LOW);
        servo.write(135);
        movementSteps++;
        movementDirection = 'I';
      }
      //Backwardleftmovement
      else if(command == 'H') {
        digitalWrite(motorForwardPin, LOW);
        digitalWrite(motorBackwardPin, HIGH);
        servo.write(45);
        movementSteps++;
        movementDirection = 'H';
      }
      //Backwardrightmovement
      else if(command == 'J') {
        digitalWrite(motorForwardPin, LOW);
        digitalWrite(motorBackwardPin, HIGH);
        servo.write(135);
        movementSteps++;
        movementDirection = 'J';
      }


        while (movementSteps > 0) {
          if (movementDirection == 'F') {
            digitalWrite(motorForwardPin, LOW);
            digitalWrite(motorBackwardPin, HIGH);
            servo.write(90);
            delay(1000);
            movementSteps--;
          } else if(movementDirection == 'B') {
            digitalWrite(motorForwardPin, HIGH);
            digitalWrite(motorBackwardPin, LOW);
            servo.write(90);
            delay(1000);
            movementSteps--;
          } else if(movementDirection == 'G') {
            digitalWrite(motorForwardPin, LOW);
            digitalWrite(motorBackwardPin, HIGH);
            servo.write(45);
            delay(1000);
            movementSteps--;
          } else if(movementDirection == 'I') {
            digitalWrite(motorForwardPin, LOW);
            digitalWrite(motorBackwardPin, HIGH);
            servo.write(135);
            delay(1000);
            movementSteps--;
          } else if(movementDirection == 'H') {
            digitalWrite(motorForwardPin, HIGH);
            digitalWrite(motorBackwardPin, LOW);
            servo.write(45);
            delay(1000);
            movementSteps--;
          } else if(movementDirection == 'J') {
            digitalWrite(motorForwardPin, HIGH);
            digitalWrite(motorBackwardPin, LOW);
            servo.write(135);
            delay(1000);
            movementSteps--;
          }
        }
        servo.write(90);
      }

      //Lighton
      else if(command == 'O') {
        digitalWrite(lightPin, HIGH);
      }
      //Lightoff
      else if(command == 'P') {
        digitalWrite(lightPin, LOW);
      }
      //Hornon
      else if(command == 'Q') {
        digitalWrite(hornPin, HIGH);
      }
      //Hornoff
      else if(command == 'W') {
        digitalWrite(hornPin, LOW);
      }
      //Stopmovement
      else if(command == 'S') {
        digitalWrite(motorForwardPin, LOW);
        digitalWrite(motorBackwardPin, LOW);
        servo.write(90);
      
    }
  }
