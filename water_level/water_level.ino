#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

const byte ROWS = 4;  //four rows
const byte COLS = 3;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 5, 6, 7, 8 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 9, 10, 11 };   //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD at 0x27 address (common); adjust if needed
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ultrasonic sensor pins
const int trigPin = 2;
const int echoPin = 3;

// Relay and buzzer pins
const int relayPin = 12;
// const int buzzerPin = 4;

// Tank parameters
const float tankDepth = 50.0;                  // cm (set to your tank height)
const float lowLevelThresholdPercent = 30.0;   // below this, pump ON
const float highLevelThresholdPercent = 70.0;  // above this, pump OFF

// Track pump state
bool pumpOn = false;

// const int redLed = 2;
// const int greenLed = 6;
// const int yellowLed = 1;

enum class MODE { IDEL,
                  AUTO,
                  TIMED };

MODE currentMode = MODE::IDEL;
unsigned long manualOffMillis = 0;
unsigned long manualOffDuration = 0;
unsigned long displayMillis = 0;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);
  // pinMode(buzzerPin, OUTPUT);

  // pinMode(redLed, OUTPUT);
  // pinMode(greenLed, OUTPUT);
  // pinMode(yellowLed, OUTPUT);

  digitalWrite(relayPin, LOW);  // Pump off initially
  // digitalWrite(buzzerPin, LOW);  // Buzzer off initially

  lcd.init();
  lcd.backlight();

  lcd.print(F("DESIGN AND"));
  lcd.setCursor(0, 1);
  lcd.print(F("CONSTRUCTION"));
  delay(3000);
  lcd.clear();

  lcd.print(F("OF AUTOMATIC WATER"));
  lcd.setCursor(0, 1);
  lcd.print(F("LEVEL CONTROL"));
  delay(3000);
  lcd.clear();

  lcd.print(F("SYSTEM USING"));
  lcd.setCursor(0, 1);
  lcd.print(F("1.5HP MACHINE"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Gutul Ronald Christopher"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/150563"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Biliyock Bright"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/149316"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Oyeyemi Henry Boluwatife"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/150501"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr.Fatima Muhammad"));
  delay(3000);
  lcd.clear();

}

void loop() {
  float distance = readUltrasonic();

  // Water level = tankDepth - distanceFromSensor
  float waterLevel = tankDepth - distance;
  if (waterLevel < 0) waterLevel = 0;
  if (waterLevel > tankDepth) waterLevel = tankDepth;

  // Calculate percentage
  int waterPercent = (waterLevel / tankDepth) * 100.0;

  // if (millis() - displayMillis > 2000) {
    // Show % on LCD
    lcd.setCursor(0, 0);
    lcd.print("Level: ");
    lcd.print(waterPercent);
    lcd.print("%    ");  // spaces clear old chars
  //   displayMillis = millis();
  // }

  char key = keypad.getKey();

  if (key == '1') {
    digitalWrite(relayPin, LOW);
    lcd.clear();
    lcd.print(F("Enter Time"));
    String value = "";
    while (1) {
      lcd.setCursor(0, 1);
      lcd.print(value);
      lcd.print(F("   "));

      char k = keypad.getKey();
      if (k >= '0' && k <= '9') {
        value += k;
      } else if (k == '*') {
        lcd.clear();
        lcd.print(F("Enter Time"));
        value = "";
      } else if (k == '#') {
        int timeValue = value.toInt();
        manualOffDuration = timeValue * 1000;
        lcd.clear();
        currentMode = MODE::TIMED;
        manualOffMillis = millis();
        pumpOn = true;
        lcd.setCursor(0, 1);
        lcd.print(F("Running..."));
        break;
      }
    }
  } else if (key == '2') {
    lcd.clear();
    lcd.print(F("Auto Mode"));
    delay(3000);
    lcd.clear();
    currentMode = MODE::AUTO;
  } else if (key == '3') {
    currentMode = MODE::IDEL;
    lcd.clear();
    pumpOn = false;
  }

  if (currentMode == MODE::TIMED && millis() - manualOffMillis > manualOffDuration) {
    pumpOn = false;

    lcd.setCursor(0, 1);
    lcd.print(F("Done...          "));

    currentMode = MODE::IDEL;
  }

  // if (waterPercent <= lowLevelThresholdPercent) {
  //   digitalWrite(redLed, HIGH);
  //   digitalWrite(greenLed, LOW);
  //   digitalWrite(yellowLed, LOW);
  // } else if (waterPercent > lowLevelThresholdPercent && waterPercent <= 70) {
  //   digitalWrite(redLed, LOW);
  //   digitalWrite(greenLed, LOW);
  //   digitalWrite(yellowLed, HIGH);
  // } else if (waterPercent > 70) {
  //   digitalWrite(redLed, LOW);
  //   digitalWrite(greenLed, HIGH);
  //   digitalWrite(yellowLed, LOW);
  // }

  if (currentMode == MODE::AUTO) {
    // Pump control with hysteresis in percentage
    if (!pumpOn && waterPercent < lowLevelThresholdPercent) {
      // Turn pump ON
      pumpOn = true;
    } else if (pumpOn && waterPercent > highLevelThresholdPercent) {
      // Turn pump OFF
      pumpOn = false;
    }

    // LCD pump status
    lcd.setCursor(0, 1);
    if (pumpOn) {
      lcd.print("Pump: ON        ");
    } else {
      lcd.print("Pump: OFF       ");
    }
  }


  // Apply pump state
  if (pumpOn) {
    digitalWrite(relayPin, HIGH);  // Pump ON
    // tone(buzzerPin, 2000);         // 2kHz tone while pump is ON
  } else {
    digitalWrite(relayPin, LOW);  // Pump OFF
    // noTone(buzzerPin);            // Buzzer OFF
  }
}

float readUltrasonic() {
  // Send pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo time
  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return tankDepth;

  // Calculate distance in cm
  float distance = duration * 0.0343 / 2;

  return distance;
}
