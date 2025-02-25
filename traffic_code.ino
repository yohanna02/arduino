const int redPin1 = 2;
const int yellowPin1 = 3;
const int greenPin1 = 4;
const int redPin2 = 5;
const int yellowPin2 = 6;
const int greenPin2 = 7;
const int redPin3 = 8;
const int yellowPin3 = 9;
const int greenPin3 = 10;
const int buttonPin = 11;

unsigned long interval = 2000;  // Set the interval for each phase in milliseconds
unsigned long previousMillis = 0;

int currentState = 0;  // 0: Lane 1, 1: Lane 2, 2: Lane 3
bool buttonPressed = false;

void setup() {
  pinMode(redPin1, OUTPUT);
  pinMode(yellowPin1, OUTPUT);
  pinMode(greenPin1, OUTPUT);
  pinMode(redPin2, OUTPUT);
  pinMode(yellowPin2, OUTPUT);
  pinMode(greenPin2, OUTPUT);
  pinMode(redPin3, OUTPUT);
  pinMode(yellowPin3, OUTPUT);
  pinMode(greenPin3, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (digitalRead(buttonPin) == LOW) {
  	delay(200);
  	buttonPressed = !buttonPressed;
  }

  if (buttonPressed) {
    // Button pressed, turn on all red lights
    digitalWrite(redPin1, HIGH);
    digitalWrite(redPin2, HIGH);
    digitalWrite(redPin3, HIGH);
    // Other lights stay off
  } else if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Turn off all lights
    digitalWrite(redPin1, LOW);
    digitalWrite(yellowPin1, LOW);
    digitalWrite(greenPin1, LOW);
    digitalWrite(redPin2, LOW);
    digitalWrite(yellowPin2, LOW);
    digitalWrite(greenPin2, LOW);
    digitalWrite(redPin3, LOW);
    digitalWrite(yellowPin3, LOW);
    digitalWrite(greenPin3, LOW);

    // Change state and control lights based on the current lane
    currentState = (currentState + 1) % 3;

    if (currentState == 0) {
      	// Lane 1
      digitalWrite(redPin1, HIGH);
      
    digitalWrite(yellowPin2, HIGH);
    
    digitalWrite(greenPin3, HIGH);
    } else if (currentState == 1) {
      // Lane 2
    	digitalWrite(yellowPin1, HIGH);
    	
    digitalWrite(greenPin2, HIGH);
    
    digitalWrite(redPin3, HIGH);
    } else if (currentState == 2) {
      // Lane 3
      digitalWrite(greenPin1, HIGH);
      
    digitalWrite(redPin2, HIGH);
    
    digitalWrite(yellowPin3, HIGH);
    }
  }
}
