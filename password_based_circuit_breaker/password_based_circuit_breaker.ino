#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

char red_phase_status[4] = "ON";
char yellow_phase_status[4] = "ON";
char blue_phase_status[4] = "ON";

#define RED_PHASE_OUTPUT 9
#define YELLOW_PHASE_OUTPUT 10
#define BLUE_PHASE_OUTPUT 11

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);

  pinMode(RED_PHASE_OUTPUT, OUTPUT);
  pinMode(YELLOW_PHASE_OUTPUT, OUTPUT);
  pinMode(BLUE_PHASE_OUTPUT, OUTPUT);

  lcd.print(F("IoT-Based"));
  lcd.setCursor(0, 1);
  lcd.print(F("Password-Protected"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Smart Circuit"));
  lcd.setCursor(0, 1);
  lcd.print(F("Breaker"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Initializing..."));
  delay(3000);
  lcd.clear();

  digitalWrite(RED_PHASE_OUTPUT, HIGH);
  digitalWrite(YELLOW_PHASE_OUTPUT, HIGH);
  digitalWrite(BLUE_PHASE_OUTPUT, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available()) {
    String cmd = Serial.readString();

    if (cmd == "RED_ON") {
      strcpy(red_phase_status, "ON");
    } else if (cmd == "RED_OFF") {
      strcpy(red_phase_status, "OFF");
    } else if (cmd == "YELLOW_ON") {
      strcpy(yellow_phase_status, "ON");
    } else if (cmd == "YELLOW_OFF") {
      strcpy(yellow_phase_status, "OFF");
    } else if (cmd == "BLUE_ON") {
      strcpy(blue_phase_status, "ON");
    } else if (cmd == "BLUE_OFF") {
      strcpy(blue_phase_status, "OFF");
    }

    Serial.readString();
  }

  if (strcmp(red_phase_status, "ON") == 0) {
    digitalWrite(RED_PHASE_OUTPUT, HIGH);
  } else {
    digitalWrite(RED_PHASE_OUTPUT, LOW);
  }

  if (strcmp(yellow_phase_status, "ON") == 0) {
    digitalWrite(YELLOW_PHASE_OUTPUT, HIGH);
  } else {
    digitalWrite(YELLOW_PHASE_OUTPUT, LOW);
  }

  if (strcmp(blue_phase_status, "ON") == 0) {
    digitalWrite(BLUE_PHASE_OUTPUT, HIGH);
  } else {
    digitalWrite(BLUE_PHASE_OUTPUT, LOW);
  }

  lcd.setCursor(0, 0);
  lcd.print(F("R"));
  lcd.setCursor(5, 0);
  lcd.print(F("Y"));
  lcd.setCursor(10, 0);
  lcd.print(F("B"));

  lcd.setCursor(0, 1);
  lcd.print(red_phase_status);
  lcd.print(F("  "));
  lcd.setCursor(5, 1);
  lcd.print(yellow_phase_status);
  lcd.print(F("  "));
  lcd.setCursor(10, 1);
  lcd.print(blue_phase_status);
  lcd.print(F("  "));
}
