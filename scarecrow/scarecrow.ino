#include <Servo.h>
#include <DFRobotDFPlayerMini.h>
#include <LiquidCrystal_I2C.h>
// #include <LiquidCrystal.h>

// --- Pin Definitions ---
#define PIR_PIN A3   // PIR sensor input
#define SERVO_PIN 5  // Servo motor pin
#define TRIG_PIN 13  // Ultrasonic trigger
#define ECHO_PIN 12  // Ultrasonic echo

// --- Config ---
#define USE_ULTRASONIC true  // <<< Set to false if you want PIR only

// --- Objects ---
Servo armServo;
DFRobotDFPlayerMini myDFPlayer;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust I2C address if needed
// LiquidCrystal lcd(2, 3, 4, 5, 6, 7);  // Adjust I2C address if needed

// --- Variables ---
unsigned long lastTrigger = 0;
unsigned long cooldown = 15000;  // 15 seconds cooldown

void setup() {
  Serial.begin(9600);  // Hardware Serial for DFPlayer Mini

  // Servo setup
  armServo.attach(SERVO_PIN);
  armServo.write(90);  // Neutral arm position

  // PIR setup
  pinMode(PIR_PIN, INPUT);

  // Ultrasonic setup
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // LCD setup
  lcd.init();
  lcd.backlight();

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("implementation"));
  delay(3000);
  lcd.clear();

  lcd.print(F("an automation"));
  lcd.setCursor(0, 1);
  lcd.print(F("scarecrow system"));
  delay(3000);
  lcd.clear();

  lcd.print(F("with motion"));
  lcd.setCursor(0, 1);
  lcd.print(F("detection,rotation"));
  delay(3000);
  lcd.clear();

  lcd.print(F("and sound features"));
  lcd.setCursor(0, 1);
  lcd.print(F("for effective crop"));
  delay(3000);
  lcd.clear();

  lcd.print(F("protection"));
  lcd.setCursor(0, 1);
  lcd.print(F("against any"));
  delay(3000);
  lcd.clear();

  lcd.print(F("intrusion"));
  delay(2000);
  lcd.clear();

  lcd.print(F("Guzam Haggai Markus"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/150741"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Lawal Abayomi Moshood"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151332"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr.Kabiru Sani"));
  delay(3000);
  lcd.clear();

  // lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Scarecrow Ready");

  // DFPlayer setup
  if (!myDFPlayer.begin(Serial)) {
    lcd.clear();
    lcd.print(F("DFPlayer Error"));
    while (true)
      ;
  }
  myDFPlayer.volume(30);  // Set volume (0–30)

  delay(3000);
  lcd.clear();
}

void loop() {
  String motion = "none";

  // PIR detection
  if (digitalRead(PIR_PIN) == HIGH) {
    motion = "pir";
  }

  // Ultrasonic detection (only if enabled)
  if (USE_ULTRASONIC) {
    long distance = getDistance();
    if (distance > 0 && distance < 10) {
      motion = "ultrasonic";
    }
  }

  // Trigger scarecrow
  if (motion != "none" && (millis() - lastTrigger > cooldown)) {
    lastTrigger = millis();
    triggerScarecrow(motion);
  }

  lcd.setCursor(0, 0);
  lcd.print(F("No Motion     "));
  lcd.setCursor(0, 1);
  lcd.print(F("detected      "));
}

void triggerScarecrow(String motion) {
  // LCD feedback
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Motion Detected!");
  lcd.setCursor(0, 1);
  lcd.print(motion);

  // Move arm up and down
  for (int i = 0; i <= 180; i++) {
    armServo.write(i);
    delay(30);
  }
  for (int i = 180; i >= 0; i--) {
    armServo.write(i);
    delay(30);
  }

  myDFPlayer.play(3);
  delay(2000);
  myDFPlayer.play(2);
  delay(2000);
  myDFPlayer.play(1);
  delay(18000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scarecrow Ready");
}


// --- Function: Ultrasonic distance ---
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // Timeout 30ms
  if (duration == 0) return -1;                    // No reading
  long distance = duration * 0.034 / 2;
  return distance;
}
