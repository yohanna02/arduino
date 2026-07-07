#define PI_SIGNAL_PIN 5
#define PI_STATUS_PIN 4
#define DIR_PIN 2
#define STEP_PIN 3

#define MIN_DELAY_US 800   // Top speed
#define MAX_DELAY_US 2000  // Start speed (Slower for high torque)
#define STEPS_PER_FRAME 180   // Big gap to prevent overlap
#define N_FRAMES 120           // Reduced to 30 as requested
#define TOTAL_STEPS 21600  // Adjust this to cover the distance you need

bool isScanning = true;
int idx = 0;

void setup() {
  pinMode(PI_SIGNAL_PIN, INPUT);
  pinMode(PI_STATUS_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(STEP_PIN, LOW);
}

void loop() {
  // int piSignal = digitalRead(PI_SIGNAL_PIN);
  if (isScanning) {
    // Inside loop()
    if (digitalRead(PI_SIGNAL_PIN) == HIGH) {
      delay(200);
      if (digitalRead(PI_SIGNAL_PIN) == LOW) { return; }
      // Move exactly one spatial resolution unit (e.g., 80 steps)
      digitalWrite(DIR_PIN, HIGH);
      for (int i = 0; i < STEPS_PER_FRAME; i++) {
        int rampDelay = MIN_DELAY_US;
        if (i < 40) rampDelay = map(i, 0, 40, MAX_DELAY_US, MIN_DELAY_US);
        if (i > 160) rampDelay = map(i, 160, 200, MIN_DELAY_US, MAX_DELAY_US);
        stepMotor();
        idx++;
      }
      // Settle time to prevent motion blur
      delay(2000);
      // Signal Pi that platform is stationary
      digitalWrite(PI_STATUS_PIN, HIGH);
      delay(100);  // Pulse width
      digitalWrite(PI_STATUS_PIN, LOW);

      // Wait for Pi to lower the move signal before allowing next step
      while (digitalRead(PI_SIGNAL_PIN) == HIGH)
        ;

      // 4. Check if we reached the end
      if (idx >= TOTAL_STEPS) {
        isScanning = false;
      }
    }
  } else {
    digitalWrite(DIR_PIN, LOW);  // Reverse
    for (long i = 0; i < idx; i++) {
      stepMotor();
    }
    idx = 0;
    isScanning = true;
    delay(5000);
  }
}

void stepMotor() {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(MIN_DELAY_US);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(MIN_DELAY_US);
}