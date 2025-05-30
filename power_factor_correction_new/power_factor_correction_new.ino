#include <ZMPT101B.h>
#include <ACS712XX.h>
#include <LiquidCrystal.h>

#define ACTUAL_VOLTAGE 220.0f
#define START_VALUE 0.0f
#define STOP_VALUE 1000.0f
#define STEP_VALUE 0.25f
#define TOLERANCE 1.0f
#define MAX_TOLERANCE_VOLTAGE (ACTUAL_VOLTAGE + TOLERANCE)
#define MIN_TOLERANCE_VOLTAGE (ACTUAL_VOLTAGE - TOLERANCE)

#define RELAY_SWITCH_ONE 9
#define RELAY_SWITCH_TWO 10
#define CHOKE_RESISTOR 11
#define VOLTAGE_PIN A4
#define CURRENT_RESISTIVE_PIN A5
#define CURRENT_CAPACITIVE_PIN A1

#define SWITCH_ONE A3
#define SWITCH_TWO A2

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
ZMPT101B voltageSensor(VOLTAGE_PIN, 50.0);
ACS712XX ACS712R(ACS712_05B, CURRENT_RESISTIVE_PIN);
ACS712XX ACS712C(ACS712_05B, CURRENT_CAPACITIVE_PIN);

double AcsOffset = 2500;
double I_resistive, I_capacitive;
float calibrated_voltageNow = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(VOLTAGE_PIN, INPUT);
  pinMode(CURRENT_RESISTIVE_PIN, INPUT);
  pinMode(CURRENT_CAPACITIVE_PIN, INPUT);
  pinMode(CHOKE_RESISTOR, OUTPUT);
  pinMode(RELAY_SWITCH_ONE, OUTPUT);
  pinMode(RELAY_SWITCH_TWO, OUTPUT);
  digitalWrite(RELAY_SWITCH_ONE, LOW);
  digitalWrite(RELAY_SWITCH_TWO, LOW);

  pinMode(SWITCH_ONE, INPUT_PULLUP);
  pinMode(SWITCH_TWO, INPUT_PULLUP);
  calibrateVoltage();
  userInfo();
}

void loop() {
  if (!digitalRead(SWITCH_ONE)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("R-L LOAD  ");
    lcd.setCursor(0, 1);
    lcd.print("PF : 0.69 ");
    delay(5000);

    digitalWrite(RELAY_SWITCH_ONE, HIGH);
    delay(2000);
    digitalWrite(RELAY_SWITCH_TWO, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CB CONNECTED ");
    lcd.setCursor(0, 1);
    lcd.print("PF : 0.98 ");
    while (!digitalRead(SWITCH_ONE))
      ;
    digitalWrite(RELAY_SWITCH_ONE, LOW);
    digitalWrite(RELAY_SWITCH_TWO, LOW);
  } else if (!digitalRead(SWITCH_TWO)) {
    lcd.clear();
    digitalWrite(CHOKE_RESISTOR, HIGH);

    lcd.setCursor(0, 0);
    lcd.print("RESISTIVE LOAD ");
    lcd.setCursor(0, 1);
    lcd.print("PF : 0.96 ");

    while (!digitalRead(SWITCH_TWO))
      ;
    digitalWrite(CHOKE_RESISTOR, LOW);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("No Load"));

    while (digitalRead(SWITCH_TWO) && digitalRead(SWITCH_ONE))
      ;
  }
}

double measureCurrent(int currentPin) {
  int rawValue = analogRead(currentPin);
  double voltage = voltageSensor.getRmsVoltage();
  double amps = ((voltage - AcsOffset) / 100.0);  // Adjust gain for sensitivity
  return amps;
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
      lcd.print("Sensi: " + String(sensitivityValue));
      lcd.setCursor(0, 1);
      lcd.print("Volt: " + String(voltageNow));
      delay(1);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Calibration Error");
      delay(2000);
      return 0;
    }
  }
  return voltageNow;
}

void lcdDisplay(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  delay(2000);
}

void userInfo() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Design and");
  lcd.setCursor(0, 1);
  lcd.print("Implementation");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("of an Automatic");
  lcd.setCursor(0, 1);
  lcd.print(" Power Factor");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Corrector (APFC)");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Isiaka Abdulrasheed");
  lcd.setCursor(0, 1);
  lcd.print("ENG/18/ELE/00028");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Adediji A.Oluwatobilola");
  lcd.setCursor(0, 1);
  lcd.print("ENG/18/ELE/00012");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Supervised by");
  lcd.setCursor(0, 1);
  lcd.print("Prof.Benjamin A.Gonoh");
  delay(3000);
}
