#include <LiquidCrystal.h>
#include <Keypad.h>

// Pin definitions
#define MQ2_PIN A0     // MQ2 gas sensor analog pin
#define BUZZER_PIN A2  // Buzzer pin

// LCD configuration
LiquidCrystal lcd(1, 2, 3, 4, 5, 6);  // RS, EN, D4, D5, D6, D7

// Keypad configuration
const byte ROWS = 4;  // Number of rows
const byte COLS = 3;  // Number of columns
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 8, 13, 12, 11 };  // Connect to the row pins of the keypad
byte colPins[COLS] = { 10, 9, 7 };       // Connect to the column pins of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int gasThreshold = 400;        // Default gas threshold
String inputBuffer = "";       // To store user input
bool setThresholdMode = true;  // Flag to toggle modes

void setup() {
  pinMode(MQ2_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.begin(16, 2);  // Initialize the LCD

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("Implementation"));
  delay(3000);
  lcd.clear();

  lcd.print(F("of alcohol"));
  lcd.setCursor(0, 1);
  lcd.print(F("detection system"));
  delay(3000);
  lcd.clear();

  // lcd.print(F("Ochogwu Charity"));
  // lcd.setCursor(0, 1);
  // lcd.print(F("BPU/EEE/H/EEC/22/"));
  // delay(3000);
  // lcd.clear();

  // lcd.print(F("Ogbebo Ede Freeman"));
  // lcd.setCursor(0, 1);
  // lcd.print(F("BPU/EEE/H/EEC/22/003"));
  // delay(3000);
  // lcd.clear();

  // lcd.print(F("Samuel Shedrac Amodu"));
  // lcd.setCursor(0, 1);
  // lcd.print(F("BPU/EEE/H/EEC/22/"));
  // delay(3000);
  // lcd.clear();

  // lcd.print(F("Supervised by"));
  // lcd.setCursor(0, 1);
  // lcd.print(F("Engr.John Odeh"));
  // delay(3000);
  // lcd.clear();
}

void loop() {
  // Read gas level
  int gasLevel = analogRead(MQ2_PIN);

  // Check for keypad input
  char key = keypad.getKey();
  if (key == '*') {
    // Toggle modes
    setThresholdMode = true;
    lcd.clear();
    if (setThresholdMode) {
      digitalWrite(BUZZER_PIN, LOW);
      lcd.print("Set Threshold:");
      inputBuffer = "";  // Clear the input buffer
    } else {
      lcd.print("Monitoring...");
    }
  } else if (setThresholdMode) {
    // Handle input in Set Threshold Mode
    lcd.setCursor(0, 0);
    digitalWrite(BUZZER_PIN, LOW);
    lcd.print("Set Threshold:      ");
    handleSetThresholdInput(key);
  }

  if (!setThresholdMode) {
    // Real-Time Monitoring Mode
    monitorGasLevel(gasLevel);
  }
}

// Function to handle input in Set Threshold Mode
void handleSetThresholdInput(char key) {
  if (key == '#') {
    // Confirm threshold input
    if (inputBuffer.length() > 0) {
      gasThreshold = inputBuffer.toInt();
      inputBuffer = "";  // Clear buffer
      lcd.clear();
      lcd.print("Threshold Set:");
      lcd.setCursor(0, 1);
      lcd.print(gasThreshold);
      delay(2000);  // Display confirmation
      lcd.clear();
      lcd.print("Set Threshold:");
      setThresholdMode = false;
    }
  } else if (key >= '0' && key <= '9') {
    // Append digit to buffer
    inputBuffer += key;
    lcd.setCursor(0, 1);
    lcd.print(inputBuffer + "  ");  // Display the current input
  }
}

// Function to monitor gas levels in Real-Time Monitoring Mode
void monitorGasLevel(int gasLevel) {
  lcd.setCursor(0, 0);
  lcd.print("Gas Level: ");
  lcd.print(gasLevel);
  lcd.print("           ");


  if (gasLevel > gasThreshold) {
    digitalWrite(BUZZER_PIN, HIGH);  // Activate buzzer
    lcd.setCursor(0, 1);
    lcd.print("ALERT! GAS HIGH!    ");
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // Deactivate buzzer
    lcd.setCursor(0, 1);
    lcd.print("Threshold: ");
    lcd.print(gasThreshold);
    lcd.print("           ");
  }
}
