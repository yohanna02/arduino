#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESPComm.h>

ESPComm esp(Serial);

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Pin definitions
#define TRIG_PIN 11
#define ECHO_PIN 12
#define PUMP_IN 4
#define PUMP_OUT 3
#define PUMP_NUTRIENT 13
#define PH_PIN A2
#define TDS_PIN A1
#define TURBIDITY_PIN A0

// Constants
const float TANK_EMPTY_CM = 18.0;
const float MIN_WATER_LEVEL_CM = 5.0;
const unsigned long PUMP_RUN_TIME = 10000;
const float PH_LOW = 3.5;
const float PH_HIGH = 6.5;
const float PH_CALIBRATION = 21.34 + 1.79;

// Variables
float temperature = 0;
float phValue = 0;
float tdsValue = 0;
float turbidityValue = 0;
float waterLevel = 0;

bool pumpInActive = false;
bool pumpOutActive = false;

unsigned long pumpStartTime = 0;
unsigned long lastSensorRead = 0;
unsigned long lastDisplay = 0;

int phBuffer[10];
byte phIndex = 0;

// pH correction state
enum PHState { PH_IDLE, PH_DRAINING, PH_REFILLING };
PHState phState = PH_IDLE;
unsigned long phStartTime = 0;

void setup() {
  esp.begin(9600);
  lcd.init();
  lcd.backlight();
  sensors.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PUMP_IN, OUTPUT);
  pinMode(PUMP_OUT, OUTPUT);
  pinMode(PUMP_NUTRIENT, OUTPUT);

  digitalWrite(PUMP_IN, LOW);
  digitalWrite(PUMP_OUT, LOW);
  digitalWrite(PUMP_NUTRIENT, LOW);

  lcd.setCursor(0, 0);
  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("construction"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("of IoT-based"));
  lcd.setCursor(0, 1);
  lcd.print(F("hydroponic system"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("for Smart"));
  lcd.setCursor(0, 1);
  lcd.print(F("farming"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("IBRAHIM MUSA"));
  lcd.setCursor(0, 1);
  lcd.print(F("HND/EE/ET/008"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("MUSTAPHA M.MAJIDADI"));
  lcd.setCursor(0, 1);
  lcd.print(F("HND/EE/ET/005"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("ABUBAKAR MUSA"));
  lcd.setCursor(0, 1);
  lcd.print(F("HND/EE/PM/005"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("HUSSAINI ADAMU"));
  lcd.setCursor(0, 1);
  lcd.print(F("HND/EE/PM/009"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("ABDULHAMID S.AKIYA"));
  lcd.setCursor(0, 1);
  lcd.print(F("HND/EE/PM/015"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr Aishatu G.Ali"));
  delay(3000);
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastSensorRead >= 1000) {
    lastSensorRead = currentMillis;
    readSensors();
  }

  handlePH(currentMillis);

  if (currentMillis - lastDisplay >= 1500) {
    lastDisplay = currentMillis;
    displayData();
  }
}

// ---------------- SENSOR FUNCTIONS ----------------
void readSensors() {
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  waterLevel = readWaterLevel();
  phValue = readPH();
  tdsValue = analogRead(TDS_PIN) * (1000.0 / 1023.0);
  turbidityValue = analogRead(TURBIDITY_PIN);
}

// pH averaging
float readPH() {
  phBuffer[phIndex] = analogRead(PH_PIN);
  phIndex = (phIndex + 1) % 10;

  int sorted[10];
  memcpy(sorted, phBuffer, sizeof(sorted));

  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (sorted[i] > sorted[j]) {
        int tmp = sorted[i];
        sorted[i] = sorted[j];
        sorted[j] = tmp;
      }
    }
  }

  unsigned long avg = 0;
  for (int i = 2; i < 8; i++) avg += sorted[i];

  float volt = (float)avg * 5.0 / 1024.0 / 6.0;
  float ph = -5.70 * volt + PH_CALIBRATION;
  return ph;
}

// Ultrasonic water level
float readWaterLevel() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  float distance = duration * 0.0343 / 2;

  float level = TANK_EMPTY_CM - distance;
  if (level < 0) level = 0;

  return level;
}

// ---------------- PH CONTROL ----------------
void handlePH(unsigned long currentMillis) {
  if ((phValue < PH_LOW || phValue > PH_HIGH) && phState == PH_IDLE && waterLevel > MIN_WATER_LEVEL_CM) {
    phState = PH_DRAINING;
    phStartTime = currentMillis;
    digitalWrite(PUMP_OUT, HIGH);
  }

  if (phState == PH_DRAINING && (currentMillis - phStartTime >= PUMP_RUN_TIME)) {
    digitalWrite(PUMP_OUT, LOW);
    phState = PH_REFILLING;
    phStartTime = currentMillis;
    digitalWrite(PUMP_IN, HIGH);
  }

  if (phState == PH_REFILLING && (currentMillis - phStartTime >= PUMP_RUN_TIME)) {
    digitalWrite(PUMP_IN, LOW);
    phState = PH_IDLE;
  }
}

// ---------------- DISPLAY FUNCTION ----------------
void displayData() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperature, 1);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("pH:");
  lcd.print(phValue, 2);
  lcd.print(" TDS:");
  lcd.print(tdsValue, 0);

  int waterpercentage = (waterLevel / TANK_EMPTY_CM) * 100;

  lcd.setCursor(0, 2);
  lcd.print("Lvl:");
  lcd.print(waterpercentage);
  lcd.print("% Turb:");
  lcd.print(turbidityValue);

  lcd.setCursor(0, 3);
  if (pumpInActive)
    lcd.print("PumpIn:ON");
  else if (phState == PH_DRAINING)
    lcd.print("Drain:ON");
  else
    lcd.print("System:OK");

  esp.send("temp", temperature);
  delay(200);
  esp.send("ph", phValue);
  delay(200);
  esp.send("tds", tdsValue);
  delay(200);
  esp.send("water_level", waterpercentage);
  delay(200);
  esp.send("turbidity", turbidityValue);
  delay(200);

}
