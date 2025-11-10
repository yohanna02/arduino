const int redPins[] = { 2, 5, 8, 11 };
const int yellowPins[] = { 3, 6, 9, 12 };
const int greenPins[] = { 4, 7, 10, 13 };
const int button = A0;
int currentStep = 0;

unsigned long previousMillis = 0;
const long intervals[] = { 5000, 2000, 5000, 2000, 5000, 2000, 5000, 2000, 5000, 2000, 5000, 2000 };

int steps[12][12] = {
  { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0 }, 
  { 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0 },
  { 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0 },
  { 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0 },
  { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0 },
  { 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0 },
  { 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0 },
  { 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0 },
  { 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0 }
};


bool buttonPressed = false;

void setup() {
  for (int i = 2; i <= 13; i++) {
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
    for (int i = 2; i <= 13; i++) {
      digitalWrite(i, LOW);
    }
  }

  if (buttonPressed) {
    for (int i = 0; i < 4; i++) {
      digitalWrite(redPins[i], HIGH);
    }
  } else if (currentMillis - previousMillis >= intervals[currentStep]) {
    previousMillis = currentMillis;

    // Turn off all LEDs
    for (int i = 2; i <= 13; i++) {
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

    digitalWrite(redPins[3], steps[currentStep][9]);
    digitalWrite(yellowPins[3], steps[currentStep][10]);
    digitalWrite(greenPins[3], steps[currentStep][11]);

    currentStep++;

    if (currentStep > 12) {
      currentStep = 0;
    }
  }
}
