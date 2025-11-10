#include <IRremote.h>
#include <LiquidCrystal.h>

// --- IR Receiver ---
#define IR_PIN 2

// --- LCD (RS, EN, D4, D5, D6, D7) ---
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

// --- Loads (Relays or LEDs) ---
#define LOAD1 9
#define LOAD2 8
#define LOAD3 7

// --- States for loads ---
bool load1State = false;
bool load2State = false;
bool load3State = false;

// --- Assign IR Remote HEX codes for each load button ---
// Replace these HEX values with the actual values from your remote
#define BTN_LOAD1 0x45  // Example: Power button
#define BTN_LOAD2 0x46  // Example: Vol+ button
#define BTN_LOAD3 0x47 // Example: Func/Stop button

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_PIN);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print(" Home Automation ");
  lcd.setCursor(0, 1);
  lcd.print("Using IR Remote");
  delay(2000);
  lcd.clear();

  // Set load pins as output
  pinMode(LOAD1, OUTPUT);
  pinMode(LOAD2, OUTPUT);
  pinMode(LOAD3, OUTPUT);

  digitalWrite(LOAD1, LOW);
  digitalWrite(LOAD2, LOW);
  digitalWrite(LOAD3, LOW);
}

void loop() {
  if (IrReceiver.decode()) {
    // --- Control Loads ---
    if (IrReceiver.decodedIRData.command == BTN_LOAD1) {
      load1State = !load1State;
      digitalWrite(LOAD1, load1State);
    }
    else if (IrReceiver.decodedIRData.command == BTN_LOAD2) {
      load2State = !load2State;
      digitalWrite(LOAD2, load2State);
    }
    else if (IrReceiver.decodedIRData.command == BTN_LOAD3) {
      load3State = !load3State;
      digitalWrite(LOAD3, load3State);
    }

    // Display load status
    lcd.setCursor(0, 0);
    lcd.print("L1:");
    lcd.print(load1State ? "ON " : "OFF");
    lcd.setCursor(0, 1);
    lcd.print("L2:");
    lcd.print(load2State ? "ON " : "OFF");
    lcd.print(" L3:");
    lcd.print(load3State ? "ON " : "OFF");

    delay(2000);
    IrReceiver.resume();
  }
}
