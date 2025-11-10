// #include <BareBoneSim800.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

// SIM800L setup
// BareBoneSim800 sim800;  // TX - 8, RX - 9
SoftwareSerial sim800(8, 9);  // TX - 8, RX - 9
// const char* phoneNumber = "+2349022107944";
const char* phoneNumber = "+2348133132748";
char* gasMsg = "Warning!!!: GAS LEAKAGE DETECTED AT HOME!";
char* fireMsg = "Warning!!!: FIRE OUTBREAK DETECTED AT HOME!";
char* bothMsg = "ALERT!!!: GAS LEAKAGE AND FIRE OUTBREAK DETECTED AT HOME!";

// LCD pins: RS, EN, D4, D5, D6, D7
// LiquidCrystal lcd(A4, A3, A2, A1, A0, 13);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions
int greenLED = 10;
int redLED = 12;
int yellowLED = 11;
int buzzerPin = 7;
int gasSensor = A3;
int fireSensor = 2;

bool smsSent = false;
int buzzerCount = 0;

void setup() {
  pinMode(gasSensor, INPUT);
  pinMode(fireSensor, INPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
  sim800.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("System Initializing");
  delay(8000);  // Allow SIM800L to connect to network

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("FIRE OUTBREAK");
  lcd.setCursor(2, 1);
  lcd.print("& LPG LEAKAGE");
  delay(3000);

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("DETECTOR");
  lcd.setCursor(7, 1);
  lcd.print("BY");
  delay(2000);

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("N/20349");
  lcd.setCursor(2, 1);
  lcd.print("LT FA ISAAC");
  delay(3000);

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("SUPERVISED");
  lcd.setCursor(7, 1);
  lcd.print("BY");
  delay(3000);

  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("COL.");
  lcd.setCursor(5, 1);
  lcd.print("APPAH");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("please wait...");
  delay(3000);

  for (int x = 1; x <= 7; x++) {
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    delay(500);

    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, HIGH);
    delay(500);

    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    digitalWrite(yellowLED, LOW);
    delay(500);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready...");
  delay(1500);

  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(buzzerPin, LOW);
}

void loop() {
  // LOW means danger detected
  bool gasDetected = (digitalRead(gasSensor) == LOW);
  bool fireDetected = (digitalRead(fireSensor) == LOW);

  lcd.clear();

  if (gasDetected && fireDetected) {
    lcd.print("GAS & FIRE ALERT!");
    dangerAction(bothMsg);
  } else if (gasDetected) {
    lcd.print("Gas Leakage!");
    dangerAction(gasMsg);
  } else if (fireDetected) {
    lcd.print("Fire Outbreak!");
    dangerAction(fireMsg);
  } else {
    lcd.print("Area Safe");
    normalState();
  }

  delay(500);
}

void dangerAction(const char* msg) {
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, HIGH);

  if (!smsSent) {
    lcd.setCursor(0, 1);
    lcd.print("Sending SMS...");
    sendSMS(String(msg), String(phoneNumber));
    // if (sim800.sendSMS(phoneNumber, msg)) {
    lcd.setCursor(0, 1);
    lcd.print("SMS: Sent OK   ");
    blinkLED(greenLED, 5, 300);
    smsSent = true;
    // } else {
    //   lcd.setCursor(0, 1);
    //   lcd.print("SMS: Failed    ");
    //   blinkLED(yellowLED, 5, 300);
    // }
  }

  if (buzzerCount == 0) {
    for (int i = 0; i < 10; i++) {
      digitalWrite(buzzerPin, HIGH);
      delay(500);
      digitalWrite(buzzerPin, LOW);
      delay(500);
    }
    buzzerCount = 1;
  }
}

void normalState() {
  digitalWrite(greenLED, HIGH);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);
  smsSent = false;
  buzzerCount = 0;
}

void blinkLED(int ledPin, int times, int duration) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(duration);
    digitalWrite(ledPin, LOW);
    delay(duration);
  }
}

void sendSMS(String message, String phoneNumbers) {
  sim800.println("AT+CMGF=1");  // Set SMS mode to text
  delay(500);
  sim800.println("AT+CMGS=\"" + phoneNumbers + "\"");  // Replace with your phone number
  delay(500);
  sim800.print(message);
  sim800.write(26);  // Ctrl+Z to send SMS
  delay(1000);
}
