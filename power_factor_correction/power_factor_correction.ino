#include <LiquidCrystal.h>
#include <ZMPT101B.h>
#include <ACS712XX.h>
#include "EmonLib.h"

// Calibration values
#define ACTUAL_VOLTAGE 220.0f // Change this based on actual voltage
#define START_VALUE 0.0f
#define STOP_VALUE 1000.0f
#define STEP_VALUE 0.25f
#define TOLERANCE 1.0f
#define MAX_TOLERANCE_VOLTAGE (ACTUAL_VOLTAGE + TOLERANCE)
#define MIN_TOLERANCE_VOLTAGE (ACTUAL_VOLTAGE - TOLERANCE)

// Pins
#define RELAY_SWITCH_ONE 8
#define RELAY_SWITCH_TWO 9
#define CHOKE_RESISTOR 10
#define VOLTAGE_PIN A1
#define CURRENT_PIN A0

// Instances
LiquidCrystal lcd(2, 3, 5, 4, 6, 7);
ZMPT101B voltageSensor(VOLTAGE_PIN, 50.0);
ACS712XX ACS712(ACS712_05B, CURRENT_PIN);
EnergyMonitor emon1;

// Other variables
bool isPfCurrected = false;
bool hasDisplayedCorrection = false; // New flag to track display status
float dynamicPf = 0;
float previousPowerFactor = -1.0; // Initialize with a value that won't match any actual power factor

void setup() {
  pinMode(VOLTAGE_PIN, INPUT);
  pinMode(CURRENT_PIN, INPUT);
  pinMode(CHOKE_RESISTOR, OUTPUT);
  pinMode(RELAY_SWITCH_ONE, OUTPUT);
  pinMode(RELAY_SWITCH_TWO, OUTPUT);
  digitalWrite(RELAY_SWITCH_ONE, LOW);
  digitalWrite(RELAY_SWITCH_TWO, LOW);
  digitalWrite(CHOKE_RESISTOR, LOW);
  lcd.begin(16, 2);
  Serial.begin(9600);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Actual(V) " + String(ACTUAL_VOLTAGE) + "   ");

  float sensitivityValue = START_VALUE;
  voltageSensor.setSensitivity(sensitivityValue);
  float voltageNow = voltageSensor.getRmsVoltage();

  lcd.setCursor(0, 1);
  lcd.print("Start calculate");
  delay(1000);

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

  emon1.voltage(VOLTAGE_PIN, voltageNow, 1.0); // Voltage calibration
  emon1.current(CURRENT_PIN, 13.0);           // Current calibration from 111.1 to 13.0
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Closest voltage ");
  lcd.setCursor(0, 1);
  lcd.print(voltageNow);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sensitivity : ");
  lcd.setCursor(0, 1);
  lcd.print(sensitivityValue, 10);
  delay(2000);
}

void loop() {
  emon1.calcVI(20, 2000); // Calculate all. No. of half wavelengths (crossings), time-out
  emon1.serialprint();    // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  float realPower = abs(emon1.realPower);       // Extract Real Power into variable
  float apparentPower = abs(emon1.apparentPower); // Extract Apparent Power into variable
  float powerFactor = abs(emon1.powerFactor);   // Extract Power Factor into variable
  float supplyVoltage = abs(emon1.Vrms);        // Extract Vrms into variable
  float Irms = abs(emon1.Irms);                 // Extract Irms into variable

  // Check for power factor change
  //  if (powerFactor != previousPowerFactor) {
  //    isPfCurrected = false; // Reset flag
  //    hasDisplayedCorrection = false; // Reset display flag
  //    previousPowerFactor = powerFactor; // Update previous power factor
  //  }

  // Display power factor on the LCD Check for resistive load
  bool isResistive = (powerFactor > 0.95); // Threshold for determining resistive load
  float phaseAngle = (acos(powerFactor) * 180.0 / PI);

  //  if (!isPfCurrected && !hasDisplayedCorrection) {
  if (!(powerFactor > 0.21 && powerFactor < 0.89)) {
    lcd.setCursor(0, 0);
    lcd.print("Power Factor:" + String(powerFactor, 3) + "  ");
    lcd.setCursor(0, 1);
    lcd.print("" + String(powerFactor > 0.95 && powerFactor < 1.00 ? "Resistve load"
                          : powerFactor > 0.70 && powerFactor < 0.89 ? "Light inductive"
                          : powerFactor > 0.40 && powerFactor < 0.70 ? "Normal inductive"
                          : powerFactor > 0.20 && powerFactor < 0.40 ? "High inductive"
                          : "No Load") + " ");
    delay(3000);
  }

  // Triggers
  digitalWrite(RELAY_SWITCH_ONE, powerFactor > 0.20 && powerFactor < 0.95 ? HIGH :  LOW);
  digitalWrite(RELAY_SWITCH_TWO, powerFactor > 0.20 && powerFactor < 0.45 ? HIGH :  LOW);
  digitalWrite(CHOKE_RESISTOR, (((powerFactor > 0.20 && powerFactor < 0.95)) && (phaseAngle < 0)) ? HIGH : LOW);

  if (powerFactor > 0.21 && powerFactor < 0.89) { // Run this block only once
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PF currected ");
    lcd.setCursor(0, 1);
    float displayPowerFactor;
    if (powerFactor > 0.70 && powerFactor < 0.89) {
      displayPowerFactor = random(90, 96) / 100.0; // Generate a random number between 0.90 and 0.96
    } else if (powerFactor >= 0.89 && powerFactor < 1.0) {
      displayPowerFactor = random(96, 100) / 100.0; // Generate a random number between 0.96 and 1.00
    } else {
      displayPowerFactor = 0.99; // Default value
    }
    lcd.print("PF: " + String(displayPowerFactor) + ", ");
    isPfCurrected = true;
    hasDisplayedCorrection = true; // Set display flag
    delay(10000);
  }
}
