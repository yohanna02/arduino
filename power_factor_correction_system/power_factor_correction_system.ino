//acs712_30a might be added
#include <ZMPT101B.h>
#include <ACS712XX.h>
#include <LiquidCrystal.h>
//needed calibration variables
// Calibration values
#define ACTUAL_VOLTAGE 220.0f // Change this based on actual voltage
#define START_VALUE 0.0f
#define STOP_VALUE 1000.0f
#define STEP_VALUE 0.25f
#define TOLERANCE 1.0f
#define MAX_TOLERANCE_VOLTAGE (ACTUAL_VOLTAGE + TOLERANCE)
#define MIN_TOLERANCE_VOLTAGE (ACTUAL_VOLTAGE - TOLERANCE)
//others
#define RELAY_SWITCH_ONE A2
#define RELAY_SWITCH_TWO A3
#define CHOKE_RESISTOR 10
#define VOLTAGE_PIN A5
#define CURRENT_PIN A4
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;

LiquidCrystal lcd (rs, en, d4, d5, d6, d7);
ZMPT101B voltageSensor(VOLTAGE_PIN, 50.0);
ACS712XX ACS712(ACS712_05B, CURRENT_PIN);

const int currentAnalogIn = A0;
int mVperAmp = 100;  // use 100 for 20A Module and 66 for 30A Module
double AcsOffset = 2500;
double I;
float calibrated_voltageNow = 0;
float calibrated_currentNow = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(VOLTAGE_PIN, INPUT);
  pinMode(CURRENT_PIN, INPUT);
  pinMode(CHOKE_RESISTOR, OUTPUT);
  pinMode(RELAY_SWITCH_ONE, OUTPUT);
  pinMode(RELAY_SWITCH_TWO, OUTPUT);
  digitalWrite(RELAY_SWITCH_ONE, LOW);
  digitalWrite(RELAY_SWITCH_TWO, LOW);
  //  caliration
  calibrateVoltage();
  //  user information
  userInfo();
}

void loop() {
  // put your main code here, to run repeatedly:
  I = measureCurrent();

  if ((I >= 0.17) && (I <= 0.24)) {
    digitalWrite(RELAY_SWITCH_ONE, LOW);
    digitalWrite(RELAY_SWITCH_TWO, LOW);
    lcd.setCursor(0, 0);
    lcd.print("NO LOAD ");
    lcd.setCursor(0, 1);
    lcd.print("CB DISCONNECTED ");
  } else if ((I >= 0.77) && (I <= 0.82)) {
    digitalWrite(RELAY_SWITCH_ONE, LOW);
    digitalWrite(RELAY_SWITCH_TWO, LOW);
    lcd.setCursor(0, 0);
    lcd.print("RESISTIVE LOAD ");
    lcd.setCursor(0, 1);
    lcd.print("PF : 0.96 ");
  } else if ((I >= 0.60) && (I <= 0.68)) {
    lcd.setCursor(0, 0);
    lcd.print("R-L LOAD  ");
    lcd.setCursor(0, 1);
    lcd.print("PF : 0.69 ");
    delay(5000);
    digitalWrite(RELAY_SWITCH_ONE, HIGH);
    delay(2000);
    digitalWrite(RELAY_SWITCH_TWO, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("CB CONNECTED ");
    lcd.setCursor(0, 1);
    lcd.print("PF : 0.98 ");
    while (!(sensor.getCurrentAC() <= 0.24));
  }
  delay(100);
}

double measureCurrent() {
  int RawValue = analogRead(CURRENT_PIN);
  //  double Voltage = (RawValue / 1024.0) * 5000;  // Gets you mV
  double Voltage = voltageSensor.getRmsVoltage();
  double Amps = ((Voltage - AcsOffset) / mVperAmp);
  return Amps;
}

float calibrateVoltage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ActualVolt: " + String(ACTUAL_VOLTAGE));

  float sensitivityValue = START_VALUE;
  voltageSensor.setSensitivity(sensitivityValue);
  float voltageNow = voltageSensor.getRmsVoltage();

  lcd.setCursor(0, 1);
  lcd.print("Starting Calc...");
  delay(2000);

  while (voltageNow > MAX_TOLERANCE_VOLTAGE || voltageNow < MIN_TOLERANCE_VOLTAGE) {
    if (sensitivityValue < STOP_VALUE) {
      sensitivityValue += STEP_VALUE;
      voltageSensor.setSensitivity(sensitivityValue);
      voltageNow = voltageSensor.getRmsVoltage();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sensi..: " + String(sensitivityValue) + "  ");
      lcd.setCursor(0, 1);
      lcd.print("Volt..: " + String(voltageNow) + "  ");
      delay(1);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Unfortunately");
      lcd.setCursor(0, 1);
      lcd.print("the sensitivity");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("value cannot be");
      lcd.setCursor(0, 1);
      lcd.print("determined");
      delay(2000);
      return;
    }
  }

  return voltageNow;
}

void userInfo() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AUTOMATIC POWER ");
  lcd.setCursor(0, 1);
  lcd.print("FACTOR CORRECTOR");
  delay(2000);
}
