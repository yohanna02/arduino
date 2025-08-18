#include <LiquidCrystal.h>

LiquidCrystal lcd(1, 2, 3, 4, 5, 6);

#define RELAY_1 9
#define RELAY_2 11
#define RELAY_3 12

#define BTN_1 7   // Frequency / Start-Stop
#define BTN_2 8   // Amplitude
#define BTN_3 13  // Pulse Width

#define MOSFET 10

int frequency = 0;
int amplitude = 0;
int pulseWidth = 0;

unsigned long lastToggleTime = 0;
bool relayState = false;
bool running = false;
int selectedRelay = 0;

unsigned long btn1PressedTime = 0;
bool btn1WasPressed = false;

void setup() {
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);

  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);

  pinMode(MOSFET, OUTPUT);
  
  lcd.begin(16, 2);

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("construction"));
  delay(3000);
  lcd.clear();

  lcd.print(F("of neuromuscular"));
  lcd.setCursor(0, 1);
  lcd.print(F("stimulator"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Ahmad Muhammad Yusuf"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/150398"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Dr Yilwa victor"));
  delay(3000);
  lcd.clear();

  lcd.print("System Ready");
  delay(1000);
  lcd.clear();
}

void loop() {
  handleButtons();
  determineRelay();

  if (running && selectedRelay > 0) {
    if (millis() - lastToggleTime >= 2000) {
      lastToggleTime = millis();
      relayState = !relayState;
      toggleRelay(relayState);
    }
  } else {
    toggleRelay(false);  // Ensure all relays OFF when stopped
  }

  displayValues();
  delay(50);
}

void handleButtons() {
  // BTN_1 - Frequency or toggle run/stop
  if (digitalRead(BTN_1) == LOW) {
    if (!btn1WasPressed) {
      btn1WasPressed = true;
      btn1PressedTime = millis();
    } else if (millis() - btn1PressedTime > 1000) {
      // Long press detected, toggle running state
      running = !running;
      relayState = false;
      toggleRelay(false);
      btn1WasPressed = false;
      delay(500);  // debounce
    }
  } else {
    if (btn1WasPressed && millis() - btn1PressedTime < 1000) {
      // Short press: Increase frequency
      frequency += 5;
      if (frequency > 100) frequency = 0;
      delay(200);
    }
    btn1WasPressed = false;
  }

  // BTN_2 - Amplitude
  if (digitalRead(BTN_2) == LOW) {
    amplitude += 20;
    if (amplitude > 300) amplitude = 0;
    delay(200);
  }

  // BTN_3 - Pulse Width
  if (digitalRead(BTN_3) == LOW) {
    pulseWidth += 30;
    if (pulseWidth > 360) pulseWidth = 0;
    delay(200);
  }
}

void determineRelay() {
  if (frequency >= 0 && frequency <= 30 && amplitude >= 0 && amplitude <= 100 && pulseWidth >= 0 && pulseWidth <= 100) {
    selectedRelay = 1;
  } else if (frequency > 30 && frequency <= 70 && amplitude > 100 && amplitude <= 200 && pulseWidth > 100 && pulseWidth <= 200) {
    selectedRelay = 2;
  } else if (frequency > 70 && frequency <= 100 && amplitude > 200 && amplitude <= 300 && pulseWidth > 200 && pulseWidth <= 300) {
    selectedRelay = 3;
  } else {
    selectedRelay = 0;  // No valid relay
  }
}

void toggleRelay(bool state) {
  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
  digitalWrite(RELAY_3, LOW);

  if (!state || selectedRelay == 0) return;

  switch (selectedRelay) {
    case 1: digitalWrite(RELAY_1, HIGH); break;
    case 2: digitalWrite(RELAY_2, HIGH); break;
    case 3: digitalWrite(RELAY_3, HIGH); break;
  }
}

void displayValues() {
  lcd.setCursor(0, 0);
  lcd.print("F:");
  lcd.print(frequency);
  lcd.print("Hz A:");
  lcd.print(amplitude);
  lcd.print("mA");

  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print(pulseWidth);
  lcd.print("us ");
  lcd.print("R:");
  lcd.print(selectedRelay);
  lcd.print(running ? " ON " : " OFF");
}
