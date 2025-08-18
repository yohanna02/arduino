// Pins for ultrasonic sensor
const int trigPin = 10;
const int echoPin = 9;

// Pin for pump (through transistor or relay)
const int pumpPin = 11;

// Distance threshold in cm
const int distanceThreshold = 10;

// Pump on duration (milliseconds)
const int pumpDuration = 3000;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);  // Ensure pump is off
  Serial.begin(9600);
}

void loop() {
  long duration;
  int distance;

  // Send ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure echo time
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance in cm
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 4 && distance <= distanceThreshold) {
    digitalWrite(pumpPin, HIGH);  // Turn on pump
    delay(pumpDuration);          // Keep pump on for set duration
    digitalWrite(pumpPin, LOW);   // Turn off pump
    delay(2000);                  // Prevent repeated triggering
  }

  delay(100);  // Small delay for stability
}
