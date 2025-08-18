#include <LiquidCrystal_I2C.h>

#define RELAY_PIN 8
#define SWITCH_PIN 12
#define TRIG_PIN 3
#define ECHO_PIN 4

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool uvAllowed = true;
bool switchState = false;
bool prevSwitchState = false;

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);  // Active LOW
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(RELAY_PIN, LOW);  // UV off

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Initializing");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Read switch
  switchState = digitalRead(SWITCH_PIN) == LOW;

  // Read ultrasonic sensor
  long duration, distance;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30ms timeout
  distance = duration * 0.034 / 2;

  bool personDetected = distance > 0 && distance < 20;

  // Safety lock
  if (personDetected) {
    digitalWrite(RELAY_PIN, LOW);
    uvAllowed = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Person Detected!");
    lcd.setCursor(0, 1);
    lcd.print("UV Disabled");
  }

  // Unlock only after switch OFF then ON
  if (!switchState && prevSwitchState) {
    uvAllowed = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Switch Reset");
    lcd.setCursor(0, 1);
    lcd.print("Ready");
    delay(1500);
  }

  // UV control
  if (switchState && uvAllowed) {
    digitalWrite(RELAY_PIN, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UV Light ON");
  } else if (!personDetected && !switchState) {
    digitalWrite(RELAY_PIN, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UV Light OFF");
  }

  prevSwitchState = switchState;
  delay(300);
}
