#define GSM_SERIAL Serial

String plateNumbers[4][2] = {
  { "KJA-825CQ", "+2348068111733" },
  { "AY174-LSR", "+2348068111733" },
  { "MNA-914JL", "+2348068111733" },
  { "BBU-333XA", "+2348068111733" }
  // { "BBU-333XA", "+2347032153315" }
};

void setup() {
  // put your setup code here, to run once:
  GSM_SERIAL.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(2)) {
    String data = "The car with " + plateNumbers[0][0] + " has violated traffic.";
    sendSMS(data, plateNumbers[0][1]);
    delay(1000);
  }
  else if (digitalRead(3)) {
    String data = "The car with " + plateNumbers[1][0] + " has violated traffic.";
    sendSMS(data, plateNumbers[1][1]);
    delay(1000);
  }
  else if (digitalRead(7)) {
    String data = "The car with " + plateNumbers[2][0] + " has violated traffic.";
    sendSMS(data, plateNumbers[2][1]);
    delay(1000);
  }
  else if (digitalRead(8)) {
    String data = "The car with " + plateNumbers[3][0] + " has violated traffic.";
    sendSMS(data, plateNumbers[3][1]);
    delay(1000);
  }
}

void sendSMS(String message, String number) {
  GSM_SERIAL.println("AT+CMGF=1");  // Set SMS mode to text
  delay(500);
  GSM_SERIAL.println("AT+CMGS=\"" + number + "\"");  // Replace with your phone number
  delay(500);
  GSM_SERIAL.print(message);
  GSM_SERIAL.write(26);  // Ctrl+Z to send SMS
  delay(1000);
}