const int EN_pin = 9;       // PWM pin controlling speed
const int PH_pin = 10;        // Digital pin controlling polarity
const int pwmVal = 128;      // 50% duty cycle (range 0–255)

const unsigned long posDuration = 100;  // Positive phase duration in ms
const unsigned long negDuration = 200;  // Negative phase duration in ms

void setup() {
  pinMode(EN_pin, OUTPUT);
  pinMode(PH_pin, OUTPUT);
  
  // Ensure proper mode settings:
  // PMODE should be tied low for PH/EN mode,
  // and SLEEP should be held high to enable the driver.
}

void loop() {
  // Positive phase
  digitalWrite(PH_pin, HIGH);
  analogWrite(EN_pin, pwmVal);
  delay(posDuration);

  // Negative phase
  digitalWrite(PH_pin, LOW);
  analogWrite(EN_pin, pwmVal);
  delay(negDuration);
}
