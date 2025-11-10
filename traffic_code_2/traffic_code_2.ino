
const int redPins[] = {2, 5, 8};
const int yellowPins[] = {3, 6, 9};
const int greenPins[] = {4, 7, 10};
const int button = 11;
int currentStep = 0;

unsigned long previousMillis = 0;
const long intervals[] = {5000, 2000, 5000, 2000, 5000, 2000, 5000, 2000, 5000};

const int steps[][9] = {
  {1, 0, 0, 1, 0, 0, 1, 0, 0},
  {1, 1, 0, 1, 0, 0, 1, 0, 0},
  {0, 0, 1, 1, 0, 0, 1, 0, 0},
  {0, 0, 1, 1, 1, 0, 1, 0, 0},
  {1, 0, 0, 0, 0, 1, 1, 0, 0},
  {1, 0, 0, 0, 0, 1, 1, 1, 0},
  {1, 0, 0, 1, 0, 0, 0, 0, 1},
  {1, 1, 0, 1, 0, 0, 0, 0, 1},
  {0, 0, 1, 1, 0, 0, 1, 0, 0}
};

bool buttonPressed = false;

void setup() {
  for (int i = 2; i <= 10; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode(button, INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();

  if (!digitalRead(button)) {
    delay(200);
    buttonPressed = !buttonPressed;
    // Turn off all LEDs
    for (int i = 2; i <= 10; i++) {
      digitalWrite(i, LOW);
    }
  }

  if (buttonPressed) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(redPins[i], HIGH);
    }
  }
  else if (currentMillis - previousMillis >= intervals[currentStep]) {
    previousMillis = currentMillis;

    // Turn off all LEDs
    for (int i = 2; i <= 9; i++) {
      digitalWrite(i, LOW);
    }

    // Turn on LEDs according to the current step
    digitalWrite(redPins[0], steps[currentStep][0]);
    digitalWrite(yellowPins[0], steps[currentStep][1]);
    digitalWrite(greenPins[0], steps[currentStep][2]);

    digitalWrite(redPins[1], steps[currentStep][3]);
    digitalWrite(yellowPins[1], steps[currentStep][4]);
    digitalWrite(greenPins[1], steps[currentStep][5]);

    digitalWrite(redPins[2], steps[currentStep][6]);
    digitalWrite(yellowPins[2], steps[currentStep][7]);
    digitalWrite(greenPins[2], steps[currentStep][8]);

    currentStep++;

    if (currentStep > 8) {
      currentStep = 0;
    }
  }
}
