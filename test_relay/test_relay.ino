#define RELAY_PIN 2
#define RELAY_PIN_2 A3

void setup() {
  // Initialize the relay pins as OUTPUT
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);

  // Turn both relays ON (assuming HIGH activates the relay)
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(RELAY_PIN_2, HIGH);

  delay(5000);
  
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(RELAY_PIN_2, LOW);

  delay(3000);
  
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(RELAY_PIN_2, HIGH);
}

void loop() {
  // Nothing in loop – relays stay ON
}
