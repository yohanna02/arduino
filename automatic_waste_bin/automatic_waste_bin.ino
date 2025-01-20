#include <LiquidCrystal.h>
#include <Servo.h>

#define TRIG_PIN_OPEN 2
#define ECHO_PIN_OPEN 3

#define TRIG_PIN_FILLED 5
#define ECHO_PIN_FILLED 4

#define SERVO_PIN 9

#define BUZZER 6
#define OPEN_DISTANCE_THRESHOLD 20  // Distance to detect an object to open the lid (in cm)
#define FULL_DISTANCE_THRESHOLD 7  // Distance indicating the bin is full (in cm)

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
Servo myServo;

bool sentEmail = false;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN_OPEN, OUTPUT);
  pinMode(TRIG_PIN_FILLED, OUTPUT);
  pinMode(ECHO_PIN_OPEN, INPUT);
  pinMode(ECHO_PIN_FILLED, INPUT);
  pinMode(BUZZER, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);  // Close the lid initially

  lcd.begin(16, 2);
  lcd.print(F("Initializing..."));
  delay(2000);
}

void loop() {
  // Measure distances
  long openDistance = measureDistance(TRIG_PIN_OPEN, ECHO_PIN_OPEN);
  long filledDistance = measureDistance(TRIG_PIN_FILLED, ECHO_PIN_FILLED);

  lcd.clear();

  // Display bin fullness
  lcd.setCursor(0, 0);
  lcd.print(F("Fullness: "));
  lcd.print(filledDistance);
  lcd.print(F(" cm"));

  if (filledDistance <= FULL_DISTANCE_THRESHOLD) {
    sentEmail = true;
    Serial.print("sending-email");
    lcd.setCursor(0, 1);
    lcd.print(F("Bin is FULL!"));
    tone(BUZZER, 1000, 1000);  // Sound the buzzer for 1 second
    noLidOpen();               // Prevent lid from opening
  } else {
    sentEmail = false;
    lcd.setCursor(0, 1);
    lcd.print(F("Bin OK"));
    noTone(BUZZER);  // Turn off the buzzer

    // Handle lid opening if bin is not full
    if (openDistance <= OPEN_DISTANCE_THRESHOLD) {
      lcd.setCursor(0, 1);
      lcd.print(F("Opening Lid..."));
      myServo.write(90);  // Open the lid
      delay(5000);        // Keep it open for 5 seconds
      myServo.write(0);   // Close the lid
    }
  }

  delay(1000);  // Delay for stability
}

long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;  // Convert to cm
  return distance;
}

void noLidOpen() {
  myServo.write(0);  // Ensure the lid stays closed
}
