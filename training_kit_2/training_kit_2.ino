#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <DHT.h>
#include <ACS712.h>

#define VOLTAGE1 A2
#define VOLTAGE2 A3

#define CURRENT1 A4
#define CURRENT2 A5

ACS712 currentSensor1(ACS712_05B, CURRENT1);
ACS712 currentSensor2(ACS712_05B, CURRENT2);

LiquidCrystal_I2C lcd(0x27, 20, 4);

DHT dht(2, DHT11);

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 3, 4, 5, 6 };   //connect to the row pinouts of the keypad
byte colPins[COLS] = { 7, 8, 9, 10 };  //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

enum class RUNNING_MODE {
  NONE,
  TRACKING,
  READING
};

RUNNING_MODE currentMode = RUNNING_MODE::NONE;

int ldr1Pin = A0;
int ldr2Pin = A1;

int in1 = 22;
int in2 = 24;
int enA = 12;

int ldr1Value, ldr2Value;
int threshold = 50;
int motorDelay = 200;  // How long motor turns per correction

float getSmoothedCurrent(ACS712& sensor, int samples = 20);
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);

void setup() {
  // put your setup code here, to run once:
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enA, OUTPUT);

  lcd.init();
  lcd.backlight();

  dht.begin();

  currentSensor1.calibrate();
  currentSensor2.calibrate();
}

void loop() {
  // put your main code here, to run repeatedly:

  while (currentMode == RUNNING_MODE::NONE) {
    lcd.setCursor(0, 0);
    lcd.print(F("Select Mode"));
    lcd.setCursor(0, 1);
    lcd.print(F("1. Tracking"));
    lcd.setCursor(0, 2);
    lcd.print(F("2. Reading"));

    char key = keypad.getKey();

    if (key == '1') {
      lcd.clear();
      currentMode = RUNNING_MODE::TRACKING;
    } else if (key == '2') {
      lcd.clear();
      currentMode = RUNNING_MODE::READING;
    }
  }

  while (currentMode == RUNNING_MODE::TRACKING) {
    ldr1Value = analogRead(ldr1Pin);
    ldr2Value = analogRead(ldr2Pin);

    lcd.setCursor(0, 0);
    lcd.print("LDR1: ");
    lcd.print(ldr1Value);
    lcd.print("    ");

    lcd.setCursor(0, 1);
    lcd.print("LDR2: ");
    lcd.print(ldr2Value);
    lcd.print("    ");

    lcd.setCursor(0, 2);
    lcd.print(F("Temp: "));
    lcd.print(dht.readTemperature());
    lcd.print(F(" C   "));

    lcd.setCursor(0, 3);
    lcd.print(F("Humid: "));
    lcd.print(dht.readHumidity());
    lcd.print(F(" %   "));

    if (ldr1Value > ldr2Value + threshold) {
      // Rotate motor to the left
      analogWrite(enA, 210);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      delay(motorDelay);
      stopMotor();
    } else if (ldr2Value > ldr1Value + threshold) {
      // Rotate motor to the right
      analogWrite(enA, 210);
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      delay(motorDelay);
      stopMotor();
    } else {
      analogWrite(enA, 0);
      stopMotor();  // No significant difference, stop motor
    }

    char key = keypad.getKey();
    if (key == 'A') {
      currentMode = RUNNING_MODE::NONE;
      lcd.clear();
    }
  }

  while (currentMode == RUNNING_MODE::READING) {
    float voltage1 = readVoltage(VOLTAGE1);
    float voltage2 = readVoltage(VOLTAGE2);
    float current1 = getSmoothedCurrent(currentSensor1);
    float current2 = getSmoothedCurrent(currentSensor2);

    lcd.setCursor(0, 0);
    lcd.print(F("V1: "));
    lcd.print(voltage1);
    lcd.print(F(" V   "));

    lcd.setCursor(0, 1);
    lcd.print(F("V2: "));
    lcd.print(voltage2);
    lcd.print(F(" V   "));

    lcd.setCursor(0, 2);
    lcd.print(F("I1: "));
    lcd.print(current1);
    lcd.print(F(" A   "));

    lcd.setCursor(0, 3);
    lcd.print(F("I2: "));
    lcd.print(current2);
    lcd.print(F(" A   "));

    char key = keypad.getKey();
    if (key == 'A') {
      currentMode = RUNNING_MODE::NONE;
      lcd.clear();
    }
  }
}

void stopMotor() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

float readVoltage(int pin) {
  float R1 = 30000.0;
  float R2 = 7500.0;
  float ref_voltage = 5.0;
  int adc_value = analogRead(pin);

  // Determine voltage at ADC input
  float adc_voltage = (adc_value * ref_voltage) / 1024.0;

  // Calculate voltage at divider input
  float in_voltage = adc_voltage * (R1 + R2) / R2;

  in_voltage = mapFloat(in_voltage, 0.0, 12.0, 0.0, 26.0);

  return in_voltage;
}

float getSmoothedCurrent(ACS712& sensor, int samples = 20) {
  float sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += sensor.getCurrentDC();
    delay(2);  // Small delay between samples
  }
  return max(0.0, sum / samples);  // Prevent negative currents
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
