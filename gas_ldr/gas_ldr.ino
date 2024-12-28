#include <LiquidCrystal.h>

// Define pin connections
const int gasSensorPin = A0;  // MQ2 sensor pin
const int ldrPin = A1;        // LDR pin
const int buzzerPin = 13;     // Buzzer pin

// Initialize LCD (RS, Enable, D4, D5, D6, D7)
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// Define gas level threshold
const int gasThreshold = 500;

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print(F("DESIGN AND"));
  lcd.setCursor(0, 1);
  lcd.print(F("CONSTRUCTION"));
  delay(3000);
  lcd.clear();

  lcd.print(F("AND TEST OF A"));
  lcd.setCursor(0, 1);
  lcd.print(F("MONITORING SYSTEM"));
  delay(3000);
  lcd.clear();

  lcd.print(F("THAT MEASURES"));
  lcd.setCursor(0, 1);
  lcd.print(F("LIGHT INTENSITY"));
  delay(3000);
  lcd.clear();

  lcd.print(F("OF GAS LEVEL"));
  delay(3000);
  lcd.clear();

  lcd.print(F("ENDURANCE U.IGNATIUS"));
  lcd.setCursor(0, 1);
  lcd.print(F("ND/SLT/21/096"));
  delay(3000);
  lcd.clear();

  lcd.print(F("THERESA ONAH OKO"));
  lcd.setCursor(0, 1);
  lcd.print(F("ND/SLT/21/095"));
  delay(3000);
  lcd.clear();

  lcd.print(F("MICHAEL C.JUDITH"));
  lcd.setCursor(0, 1);
  lcd.print(F("ND/SLT/21/091"));
  delay(3000);
  lcd.clear();

  // Set buzzer pin as output
  pinMode(buzzerPin, OUTPUT);

  // Allow sensor to warm up
  delay(2000);
  lcd.clear();
}

void loop() {
  // Read gas level
  int gasLevel = analogRead(gasSensorPin);

  // Read LDR value and map it to a scale of 0 to 100
  int ldrValue = analogRead(ldrPin);
  int mappedLdrValue = map(ldrValue, 0, 1023, 0, 100);

  // Display gas level on LCD
  lcd.setCursor(0, 0);
  lcd.print("Gas: ");
  lcd.print(gasLevel);
  lcd.print("    ");  // Clear leftover digits

  // Check if gas level exceeds threshold
  if (gasLevel > gasThreshold) {
    // Activate buzzer
    digitalWrite(buzzerPin, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Gas High!      ");
  } else {
    // Deactivate buzzer
    digitalWrite(buzzerPin, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Light: ");
    lcd.print(mappedLdrValue);
    lcd.print("%   ");  // Clear leftover digits
  }

  // Short delay before repeating
  delay(500);
}