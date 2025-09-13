// Include the LiquidCrystal library for controlling the LCD
#include <LiquidCrystal.h>

// Initialize the LiquidCrystal library with the numbers of the interface pins
// LCD pin names:   RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(8, 7, 9, 10, 11, 12);

// Define the analog pin connected to the voltage divider output
const int voltagePin = A0;

// Define the resistor values used in the voltage divider
const float R1 = 6800.0;  // Resistance of R1 (top resistor) in Ohms
const float R2 = 4700.0;  // Resistance of R2 (bottom resistor) in Ohms

// Define the battery's voltage range for percentage calculation
// These values are typical for a 12V lead-acid battery
const float fullVoltage = 12.0;   // Corresponds to 100%
const float emptyVoltage = 0.0;  // Corresponds to 0%

void setup() {
  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);

  // Print a message to the LCD at startup
  lcd.setCursor(0, 0);
  lcd.print("12V Battery");
  lcd.setCursor(0, 1);
  lcd.print("Level Indicator");
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Timothy Samson");
  lcd.setCursor(0, 1);
  lcd.print("Reuben Bitrus");
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Dungji Jonathan");
  lcd.setCursor(0, 1);
  lcd.print("Isaac Audu Uren");
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Naphtali Hassan");
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Supervised by");
  lcd.setCursor(0, 1);
  lcd.print("Engr.Abdulkarim Salama");
  delay(3000);
  lcd.clear();

  // You can also use the Serial Monitor for debugging
  // Serial.begin(9600);
}

void loop() {
  // Read the raw analog value from the pin (0-1023 range)
  int sensorValue = analogRead(voltagePin);

  // Convert the analog reading into a voltage at the pin (V_out)
  float V_out = sensorValue * (5.0 / 1024.0);

  // Calculate the actual input voltage (V_in) using the voltage divider formula
  float V_in = V_out * ((R1 + R2) / R2);

  // Calculate the percentage
  // First, subtract the minimum voltage to get the "useful" voltage range
  // Then, divide by the total range and multiply by 100 to get a percentage
  float voltageRange = fullVoltage - emptyVoltage;
  float percentage = ((V_in - emptyVoltage) / voltageRange) * 100.0;

  // Ensure the percentage stays within 0-100% even if readings fluctuate slightly
  if (percentage > 100.0) {
    percentage = 100.0;
  }
  if (percentage < 0.0) {
    percentage = 0.0;
  }

  // Clear the LCD screen and display the results
  // lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Input:");
  lcd.setCursor(7, 0);
  lcd.print(V_in, 2);  // Print voltage with 2 decimal places
  lcd.print(" V     ");

  // Display the percentage on the second line
  lcd.setCursor(0, 1);
  lcd.print("Level:");
  lcd.print(percentage, 0);  // Print percentage as a whole number
  lcd.print(" %     ");

  // Wait a moment before updating the display again
  delay(1000);
}