#include <TimerOne.h>

const int pwmPin = 9; // Pin 9 corresponds to Timer1's PWM output

void setup() {
  pinMode(pwmPin, OUTPUT);
  setFrequency(50);    // Set initial frequency to 50 Hz
  setPulseWidth(200, 50);  // Set initial pulse width to 200 µs
}

void loop() {
  // Example: Adjust frequency and pulse width dynamically
  // setFrequency(newFrequency);
  // setPulseWidth(newPulseWidth);
}

// Function to set the PWM frequency (in Hz)
void setFrequency(float frequency) {
  // Calculate the period in microseconds
  float period = 1000000.0 / frequency;
  Timer1.initialize(period); // Set Timer1 period
}

// Function to set the pulse width (ON time in microseconds)
void setPulseWidth(float pulseWidth, float frequency) {
  // Calculate duty cycle as a fraction of the period
  float period = 1000000.0 / frequency;
  float dutyCycle = pulseWidth / period;
  Timer1.pwm(pwmPin, dutyCycle * 1023); // Set PWM with duty cycle
}
