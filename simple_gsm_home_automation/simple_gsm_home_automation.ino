String buffer = "";
String messageText = "";

#define RELAY_PIN 7
String fixedNumber = "+2348012345678"; // replace with your phone number

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  Serial.begin(9600);
  delay(1000);
  Serial.println("AT");
  delay(500);
  Serial.println("AT+CMGF=1");
  delay(500);
  Serial.println("AT+CNMI=1,2,0,0,0");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    buffer += c;
    if (c == '\n') {
      if (buffer.length() > 2 && buffer.indexOf("+CMT:") == -1) {
        messageText = buffer;
        messageText.trim();
        handleCommand(messageText);
      }
      buffer = "";
    }
  }
}

void handleCommand(String text) {
  text.toUpperCase();

  if (text.indexOf("LIGHT ON") != -1) {
    digitalWrite(RELAY_PIN, HIGH);
    sendAck("Light turned ON");
  } 
  else if (text.indexOf("LIGHT OFF") != -1) {
    digitalWrite(RELAY_PIN, LOW);
    sendAck("Light turned OFF");
  } 
  else {
    sendAck("Unknown command");
  }
}

void sendAck(String message) {
  Serial.println("AT+CMGF=1");
  delay(500);
  Serial.print("AT+CMGS=\"");
  Serial.print(fixedNumber);
  Serial.println("\"");
  delay(500);
  Serial.print(message);
  Serial.write(26);
  delay(1000);
}

