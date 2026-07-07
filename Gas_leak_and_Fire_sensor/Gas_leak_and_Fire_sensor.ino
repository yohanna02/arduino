#include <LiquidCrystal.h>
#include <DHT.h>

// ================= SIM800L =================
#define sim800 Serial
String phoneNumber[2] = {"+2348138271272", "+2348148713266"};

char* gasMsg   = "Warning!!!: GAS LEAKAGE DETECTED!";
char* fireMsg  = "Warning!!!: FIRE OUTBREAK DETECTED!";
char* bothMsg  = "ALERT!!!: GAS & FIRE DETECTED!";
char* motionMsg= "Alert!!!: INTRUDER DETECTED!";
char* tempMsg  = "Warning!!!: HIGH TEMPERATURE DETECTED!";

// ================= LCD (NORMAL) =================
LiquidCrystal lcd(A1, A2, A3, A4, A5, 12);

// ================= SENSORS =================
int gasSensor = A0;   // moved to avoid LCD conflict
int fireSensor = 3;
int pirSensor = 2;

// ================= DHT =================
#define DHTPIN 11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ================= FLAGS =================
bool smsSent = false;

void setup() {
  pinMode(gasSensor, INPUT);
  pinMode(fireSensor, INPUT);
  pinMode(pirSensor, INPUT);

  sim800.begin(9600);

  lcd.begin(16, 2);

  dht.begin();

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("construction"));
  delay(3000);
  lcd.clear();

  lcd.print(F("of Home"));
  lcd.setCursor(0, 1);
  lcd.print(F("security system"));
  delay(3000);
  lcd.clear();

  lcd.print(F("OSENI ABDULBAKI"));
  lcd.setCursor(0, 1);
  lcd.print(F("024/0039"));
  delay(3000);
  lcd.clear();

  lcd.print(F("GODWIN G.AFRANPA"));
  lcd.setCursor(0, 1);
  lcd.print(F("0040/0041"));
  delay(3000);
  lcd.clear();

  lcd.clear();
  lcd.print("Initializing...");
  delay(8000); // allow SIM800 network

  lcd.clear();
  lcd.print("System Ready");
  delay(2000);
}

void loop() {

  // ===== READ SENSORS =====
  bool gasDetected = (digitalRead(gasSensor) == LOW);
  bool fireDetected = (digitalRead(fireSensor) == LOW);
  bool motionDetected = (digitalRead(pirSensor) == HIGH);

  float temperature = dht.readTemperature();

  lcd.clear();

  // ===== DHT ERROR CHECK =====
  if (isnan(temperature)) {
    lcd.print("DHT Error!");
    delay(1000);
    return;
  }

  bool highTemp = (temperature > 45); // adjust if needed

  // ===== CONDITIONS =====

  if (gasDetected && fireDetected && motionDetected) {
    lcd.print("CRITICAL ALERT!");
    dangerAction("CRITICAL!!! GAS FIRE INTRUDER!");
  }

  else if (gasDetected && fireDetected) {
    lcd.print("GAS & FIRE!");
    dangerAction(bothMsg);
  }

  else if (gasDetected) {
    lcd.print("Gas Leakage!");
    dangerAction(gasMsg);
  }

  else if (fireDetected) {
    lcd.print("Fire Outbreak!");
    dangerAction(fireMsg);
  }

  else if (highTemp) {
    lcd.print("High Temp!");
    dangerAction(tempMsg);
  }

  else if (motionDetected) {
    lcd.print("Motion Detected!");
    dangerAction(motionMsg);
  }

  else {
    lcd.print("Area Safe");

    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(temperature);
    lcd.print("C");

    smsSent = false;
  }

  delay(1000);
}

// ===== ALERT FUNCTION =====
void dangerAction(const char* msg) {
  if (!smsSent) {
    lcd.setCursor(0, 1);
    lcd.print("Sending SMS...");
    sendSMS(String(msg), phoneNumber[0]);
    sendSMS(String(msg), phoneNumber[1]);

    lcd.setCursor(0, 1);
    lcd.print("SMS Sent OK   ");
    smsSent = true;
  }
}

// ===== SEND SMS =====
void sendSMS(String message, String phoneNumbers) {
  sim800.println("AT+CMGF=1");
  delay(500);

  sim800.println("AT+CMGS=\"" + phoneNumbers + "\"");
  delay(500);

  sim800.print(message);
  sim800.write(26); // CTRL+Z
  delay(1000);
}