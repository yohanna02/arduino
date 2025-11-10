#include <LiquidCrystal.h>
#include <max6675.h>  // Thermocouple library

// ---------------- PIN DEFINITIONS ----------------
#define RED_INPUT 8
#define YELLOW_INPUT 12
#define BLUE_INPUT 13

#define RED_RELAY 9
#define YELLOW_RELAY 10
#define BLUE_RELAY 11

// Thermocouple MAX6675 pins
#define thermoSO 3
#define thermoCS 5
#define thermoSCK 6

#define sim800 Serial

// LCD (4-bit mode with A0-A5 as pins)
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

// Thermocouple object
MAX6675 thermocouple(thermoSCK, thermoCS, thermoSO);

// ---------------- VARIABLES ----------------
float temperature = 0.0;
const float TEMP_THRESHOLD = 40.0;  // adjust as needed
bool tempSmsSent = false;           // prevent multiple SMS spam
bool redSmsSent = true;
bool yellowSmsSent = true;
bool blueSmsSent = true;

// ---------------- FUNCTIONS ----------------
void sendSMS(String message) {
  sim800.println("AT+CMGF=1");  // Set SMS mode to text
  delay(500);
  sim800.println("AT+CMGS=\"+2348148593727\"");  // Replace with your phone number
  delay(500);
  sim800.print(message);
  sim800.write(26);  // Ctrl+Z to send SMS
  delay(2000);
}

void setup() {
  // Phase input pins
  pinMode(RED_INPUT, INPUT);
  pinMode(YELLOW_INPUT, INPUT);
  pinMode(BLUE_INPUT, INPUT);

  // Relay output pins
  pinMode(RED_RELAY, OUTPUT);
  pinMode(YELLOW_RELAY, OUTPUT);
  pinMode(BLUE_RELAY, OUTPUT);

  sim800.begin(9600);

  // LCD
  lcd.begin(16, 2);
  
  lcd.print("Design and");
  lcd.setCursor(0, 1);
  lcd.print("construction");
  delay(3000);
  lcd.clear();

  lcd.print("of an induction");
  lcd.setCursor(0, 1);
  lcd.print("motor protection");
  delay(3000);
  lcd.clear();

  lcd.print("system from phase");
  lcd.setCursor(0, 1);
  lcd.print("failure and high");
  delay(3000);
  lcd.clear();

  lcd.print("temperature with");
  lcd.setCursor(0, 1);
  lcd.print("G.S.M");
  delay(3000);
  lcd.clear();

  lcd.print("Abdul-Rafiu Mubarak Ayinde");
  lcd.setCursor(0, 1);
  lcd.print("23/149245");
  delay(3000);
  lcd.clear();

  lcd.print("Daniel Aron");
  lcd.setCursor(0, 1);
  lcd.print("23/149436");
  delay(3000);
  lcd.clear();

  lcd.print("Abdul-Rahman Daiza");
  lcd.setCursor(0, 1);
  lcd.print("23/149331");
  delay(3000);
  lcd.clear();

  lcd.print("Okoh Joseph");
  lcd.setCursor(0, 1);
  lcd.print("23/149629");
  delay(3000);
  lcd.clear();

  lcd.print("Supervised by");
  lcd.setCursor(0, 1);
  lcd.print("Mr.Joel Mark");
  delay(3000);
  lcd.clear();
}

void loop() {
  unsigned long turnedOn = millis();
  // ----------- READ PHASE STATUS -----------
  bool redPhase = digitalRead(RED_INPUT);
  bool yellowPhase = digitalRead(YELLOW_INPUT);
  bool bluePhase = digitalRead(BLUE_INPUT);

  // Control relays based on phase availability
  digitalWrite(RED_RELAY, redPhase);
  digitalWrite(YELLOW_RELAY, yellowPhase);
  digitalWrite(BLUE_RELAY, bluePhase);

  // ----------- READ TEMPERATURE -----------
  temperature = thermocouple.readCelsius();

  // ----------- DISPLAY ON LCD -----------
  lcd.setCursor(0, 0);
  lcd.print("R:");
  lcd.print(redPhase ? "ON " : "OFF");
  lcd.print(" Y:");
  lcd.print(yellowPhase ? "ON " : "OFF");

  lcd.setCursor(0, 1);
  lcd.print("B:");
  lcd.print(bluePhase ? "ON " : "OFF ");
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C ");

  // --- Temperature Alert ---
  if (temperature > TEMP_THRESHOLD && !tempSmsSent) {
    lcd.clear();
    lcd.print(F("Temp High"));
    lcd.setCursor(0, 1);
    lcd.print(F("Sending SMS"));
    sendSMS("ALERT\nTemperature High! (" + String(temperature) + "C)");
    delay(3000);
    lcd.clear();
    tempSmsSent = true;
  } else if (temperature <= TEMP_THRESHOLD - 2) {  // add small hysteresis
    tempSmsSent = false;                           // reset only when temp goes safely down
  }

  // --- Phase Alerts ---
  if (!redPhase && !redSmsSent) {
    lcd.clear();
    lcd.print(F("Red Phase OFF"));
    lcd.setCursor(0, 1);
    lcd.print(F("Sending SMS"));
    sendSMS("ALERT\nRed Phase OFF");
    redSmsSent = true;
    delay(3000);
    lcd.clear();
  } else if (redPhase) {
    redSmsSent = false;
  }

  if (!yellowPhase && !yellowSmsSent) {
    lcd.clear();
    lcd.print(F("Yellow Phase OFF"));
    lcd.setCursor(0, 1);
    lcd.print(F("Sending SMS"));
    sendSMS("ALERT\nYellow Phase OFF");
    delay(3000);
    lcd.clear();
    yellowSmsSent = true;
  } else if (yellowPhase) {
    yellowSmsSent = false;
  }

  if (!bluePhase && !blueSmsSent) {
    lcd.clear();
    lcd.print(F("Blue Phase OFF"));
    lcd.setCursor(0, 1);
    lcd.print(F("Sending SMS"));
    sendSMS("ALERT\nBlue Phase OFF");
    delay(3000);
    lcd.clear();
    blueSmsSent = true;
  } else if (bluePhase) {
    blueSmsSent = false;
  }

  delay(1000);
}
