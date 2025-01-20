#include <LiquidCrystal.h>

// Initialize the LCD with pin connections
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// Define pin numbers for switches and LDR sensor
const int switch1 = 9;
const int switch2 = 10;
const int switch3 = 11;
const int ldrPin = A0;
const int ldrPin2 = A1;

void setup() {
  // Set up the LCD
  lcd.begin(16, 2);

  // Configure switches as input with pull-up resistors
  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(switch3, INPUT_PULLUP);

  pinMode(ldrPin, INPUT);
  pinMode(ldrPin2, INPUT);

  lcd.print(F("construction"));
  lcd.setCursor(0, 1);
  lcd.print(F("of mini solar"));
  delay(3000);
  lcd.clear();
  
  lcd.print(F("trainer"));
  lcd.setCursor(0, 1);
  lcd.print(F("panel"));
  delay(3000);
  lcd.clear();
  
  lcd.print(F("Francis E.Abu"));
  lcd.setCursor(0, 1);
  lcd.print(F("EEE/ H/ EEP/ 22/015"));
  delay(3000);
  lcd.clear();
  
  lcd.print(F("Oko P.Adakole"));
  lcd.setCursor(0, 1);
  lcd.print(F("Ochoche Mathias"));
  delay(3000);
  lcd.clear();
  
  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Mrs Adoyi Hellen"));
  delay(3000);
  lcd.clear();

  // Display initial message
  lcd.print("LDR Value:");
}

void loop() {
  // Read the LDR value
  int ldrValue = analogRead(ldrPin);
  int ldrValue2 = analogRead(ldrPin2);

  // Check if the first switch is pressed
  if (digitalRead(switch1) == LOW) {
    showWelcomeMessage();
  } else {
    // Display the LDR value continuously
    lcd.setCursor(0, 0);
    lcd.print(F("LDR 1: "));
    lcd.print(ldrValue);
    lcd.print("     "); // Clear extra characters
    lcd.setCursor(0, 1);
    lcd.print(F("LDR 2: "));
    lcd.print(ldrValue2);
    lcd.print("     "); // Clear extra characters
  }

  // Check if the second switch is pressed
  if (digitalRead(switch2) == LOW) {
    showSeriesConnection();
  }

  // Check if the third switch is pressed
  if (digitalRead(switch3) == LOW) {
    showParallelConnection();
  }
}

void showWelcomeMessage() {
  lcd.clear();
  lcd.print("Welcome!");
  delay(2000); // Wait for 2 seconds
  lcd.clear();
  lcd.print("Press a switch");
}

void showSeriesConnection() {
  lcd.clear();
  lcd.print("Series Conn:");
  delay(2000);

  lcd.clear();
  lcd.print("Step 1: Connect");
  lcd.setCursor(0, 1);
  lcd.print("B1+/S1+ to");
  delay(3000);

  lcd.clear();
  lcd.print("B2-/S2-");
  lcd.setCursor(0, 1);
  lcd.print("respectively");
  delay(3000);

  lcd.clear();
  lcd.print("Verify the conn");
  lcd.setCursor(0, 1);
  lcd.print("is Series");
  delay(3000);

  lcd.clear();
  lcd.print("Press a switch");
}

void showParallelConnection() {
  lcd.clear();
  lcd.print("Parallel Conn:");
  delay(2000);

  lcd.clear();
  lcd.print("Step 1: Connect");
  lcd.setCursor(0, 1);
  lcd.print("B1+/S1+ to");
  delay(3000);

  lcd.clear();
  lcd.print("B2+/S2+");
  delay(3000);

  lcd.clear();
  lcd.print("Step 2: Connect");
  lcd.setCursor(0, 1);
  lcd.print("B1-/S1- to");
  delay(3000);

  lcd.clear();
  lcd.print("B2-/S2-");
  delay(3000);

  lcd.clear();
  lcd.print("Verify the conn");
  lcd.setCursor(0, 1);
  lcd.print("is parallel");
  delay(3000);

  lcd.clear();
  lcd.print("Press a switch");
}
