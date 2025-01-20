#define PWM_PIN_1 9   // Pin for first phase of biphasic signal
#define PWM_PIN_2 10  // Pin for second phase of biphasic signal
#define FREQ_PIN A0   // Pin for frequency adjustment (potentiometer)
#define PW_PIN A1     // Pin for pulse width adjustment (potentiometer)
#define AMP_PIN A2    // Pin for amplitude adjustment (potentiometer)

unsigned int frequency = 30; // Default frequency (Hz)
unsigned int pulseWidth = 180; // Default pulse width (microseconds)
unsigned int amplitude = 100;  // Default amplitude (mA)
unsigned long period; // Signal period in microseconds
unsigned long pulseDelay; // Pulse delay time in microseconds
unsigned long lastSerialUpdate = 0; // Timer for periodic serial updates
const unsigned long serialInterval = 500; // Update interval in milliseconds

void setup() {
  pinMode(PWM_PIN_1, OUTPUT);
  pinMode(PWM_PIN_2, OUTPUT);
  analogWrite(PWM_PIN_1, 0);
  analogWrite(PWM_PIN_2, 0);
  
  Serial.begin(9600); // Initialize serial communication
  Serial.println("Biphasic Signal Generator");
}

void loop() {
  // Read input values
  frequency = map(analogRead(FREQ_PIN), 0, 1023, 0, 60); // Map frequency from 0 to 60 Hz
  pulseWidth = map(analogRead(PW_PIN), 0, 1023, 0, 360); // Map pulse width from 0 to 360 microseconds
  amplitude = map(analogRead(AMP_PIN), 0, 1023, 0, 255); // Map amplitude from 0 to max PWM value (255)

  // Calculate period and pulse delay
  if (frequency > 0) {
    period = 1000000 / frequency; // Period in microseconds
    pulseDelay = (period - 2 * pulseWidth) / 2; // Delay between pulses
  } else {
    period = 0;
    pulseDelay = 0;
  }

  // Generate biphasic signal
  if (frequency > 0 && pulseWidth > 0) {
    // Phase 1: Positive pulse
    analogWrite(PWM_PIN_1, amplitude);
    analogWrite(PWM_PIN_2, 0);
    delayMicroseconds(pulseWidth);

    // Inter-pulse delay
    analogWrite(PWM_PIN_1, 0);
    analogWrite(PWM_PIN_2, 0);
    delayMicroseconds(pulseDelay);

    // Phase 2: Negative pulse
    analogWrite(PWM_PIN_1, 0);
    analogWrite(PWM_PIN_2, amplitude);
    delayMicroseconds(pulseWidth);

    // Rest of the period
    analogWrite(PWM_PIN_1, 0);
    analogWrite(PWM_PIN_2, 0);
    delayMicroseconds(pulseDelay);
  } else {
    // If frequency or pulse width is zero, turn off outputs
    analogWrite(PWM_PIN_1, 0);
    analogWrite(PWM_PIN_2, 0);
  }

  // Update serial output every `serialInterval` milliseconds
  if (millis() - lastSerialUpdate >= serialInterval) {
    lastSerialUpdate = millis();
    Serial.print("Frequency: ");
    Serial.print(frequency);
    Serial.print(" Hz, Pulse Width: ");
    Serial.print(pulseWidth);
    Serial.print(" us, Amplitude: ");
    Serial.print(map(amplitude, 0, 255, 0, 167)); // Map PWM value to current in mA
    Serial.println(" mA");
  }
}
