const int appliance1Pin = 7; // Connected to relay 1
const int appliance2Pin = 8; // Connected to relay 2

void setup() {
  Serial.begin(9600);
  pinMode(appliance1Pin, OUTPUT);
  pinMode(appliance2Pin, OUTPUT);
  digitalWrite(appliance1Pin, LOW);
  digitalWrite(appliance2Pin, LOW);
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "A1ON") {
      digitalWrite(appliance1Pin, HIGH);
    } else if (command == "A1OFF") {
      digitalWrite(appliance1Pin, LOW);
    } else if (command == "A2ON") {
      digitalWrite(appliance2Pin, HIGH);
    } else if (command == "A2OFF") {
      digitalWrite(appliance2Pin, LOW);
    }
  }
}

