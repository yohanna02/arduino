const int pwmPin = 9;
const int in2Pin = 8;

unsigned long previousMicros = 0; // Use micros() for better resolution
unsigned long periodMicros = 16667; // Initial period (60 Hz) in microseconds
int amplitudeValue = 0;           // Initial amplitude (0 mA)
unsigned long pulseWidthMicros = 0; // Initial pulse width (0 us)

// Control variables (normalized)
float amplitudeControl = 1.0; // 0.0 - 1.0 (0-167mA)
float frequencyControl = 1.0; // 0.0 - 1.0 (0-60Hz)
float pulseWidthControl = 1.0; // 0.0 - 1.0 (0-360us)

// DRV8871 and Current Limiting
const float maxVoltage = 12.0; // Example: Supply voltage to DRV8871 (VM)
const float currentSenseResistor = 10.0; // Example: Current sense resistor (Ohms)
const float maxCurrent = maxVoltage / currentSenseResistor; // Max current
const int maxAnalogWrite = 255; // Max value for analogWrite

void setup() {
  pinMode(pwmPin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  digitalWrite(in2Pin, LOW); // Tie IN2 to GND
  Serial.begin(115200); // Use a higher baud rate for faster communication
  Serial.println("begin");
}

void loop() {
  unsigned long currentMicros = micros();

  // Update parameters based on control variables
  float frequencyHz = frequencyControl * 60.0; // Scale frequency
  if (frequencyHz > 0) {
    periodMicros = 1000000.0 / frequencyHz; // Period in microseconds
  } else {
    periodMicros = 16667; // Set a default period if frequency is 0
  }

  pulseWidthMicros = pulseWidthControl * 360.0; // Scale pulse width

  // Calculate the analogWrite value to limit current
  float desiredCurrent = amplitudeControl * 0.167; // Scale amplitude (0-0.167A)
  float desiredVoltage = desiredCurrent * currentSenseResistor;
  amplitudeValue = mapFloat(desiredVoltage, 0, maxVoltage, 0, maxAnalogWrite);
  amplitudeValue = constrain(amplitudeValue, 0, maxAnalogWrite);

  if (currentMicros - previousMicros >= periodMicros / 2) {
    previousMicros = currentMicros;

    // Set the pin HIGH
    digitalWrite(pwmPin, HIGH);
    analogWrite(pwmPin, amplitudeValue); // Set amplitude (PWM duty cycle)

    // Wait for the HIGH time
    delayMicroseconds(pulseWidthMicros);

    // Set the pin LOW
    digitalWrite(pwmPin, LOW);

    // Wait for the remaining time (approximately)
    delayMicroseconds(periodMicros / 2 - pulseWidthMicros);
  }
}

// Helper function for float mapping
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
