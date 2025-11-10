#include <SoftwareSerial.h>

SoftwareSerial gsm(2, 3);

String plateNumbers[4][2] = {
  { "GRE-165RC", "+2349022107944" },
  { "KJA-825CQ", "+2349022107944" },
  { "TRR-791YX", "+2349022107944" },
  { "BBU-333XA", "+2349022107944" }
};

#define GSM_SERIAL gsm

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  GSM_SERIAL.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String plateNumber = Serial.readStringUntil(' ');
    float speed = Serial.readString().toFloat();

    for (int i = 0; i < 4; i++) {
      if (plateNumbers[i][0] == plateNumber) {
        String data = "The car with " + plateNumber + " has violated traffic. Speed: " + String(speed);
        send_sms(data, plateNumbers[i][1]);
      }
    }
  }
}

void send_sms(const char *sms_data, const char *number) {
  GSM_SERIAL.println("AT+CMGF=1");
  delay(1000);
  char num_data[30];
  sprintf(num_data, "AT+CMGS=\"%s\"", number);
  GSM_SERIAL.println(num_data);
  delay(1000);
  GSM_SERIAL.print(sms_data);
  delay(1000);
  GSM_SERIAL.write(26);
  delay(1000);
}
