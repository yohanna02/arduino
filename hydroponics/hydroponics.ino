#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Pin definitions
#define TRIG_PIN 9
#define ECHO_PIN 8
#define HEATER 12
#define PUMP_IN 2
#define PUMP_OUT 3
#define PUMP_NUTRIENT 11
#define PH_PIN A1
#define TDS_PIN A2

// Constants
const float TANK_EMPTY_CM = 18.0;
const float MIN_WATER_LEVEL_CM = 5.0;
const float DESIRED_TEMP = 25.0;
const unsigned long HEATER_ON_TIME = 10000;  // 10 seconds
const unsigned long PUMP_RUN_TIME = 10000;    // 5 seconds
const float PH_LOW = 3.5;
const float PH_HIGH = 6.5;
const float PH_CALIBRATION = 21.34 + 1.79;

// Variables
float temperature = 0;
float phValue = 0;
float tdsValue = 0;
float waterLevel = 0;

bool heaterOn = false;
bool pumpInActive = false;
bool pumpOutActive = false;
bool afterHeatingPump = false;

unsigned long heaterStartTime = 0;
unsigned long pumpStartTime = 0;
unsigned long lastSensorRead = 0;
unsigned long lastDisplay = 0;

int phBuffer[10];
byte phIndex = 0;

// pH correction state
enum PHState { PH_IDLE,
               PH_DRAINING,
               PH_REFILLING };
PHState phState = PH_IDLE;
unsigned long phStartTime = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  sensors.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(PUMP_IN, OUTPUT);
  pinMode(PUMP_OUT, OUTPUT);
  pinMode(PUMP_NUTRIENT, OUTPUT);

  digitalWrite(HEATER, LOW);
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
  lcd.print(F("of hydroponics"));
  lcd.setCursor(0, 1);
  lcd.print(F("system"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Bill Clinton Abdou"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/1111/0014"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Abubakar Salisu Maitaya"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/1111B/0016"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Amyinidai David"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/1111B/0014"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Abdulkakir Abdulahi"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/1111/0025"));
  delay(3000);
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();

  // --- 1. Read sensors every 1s ---
  if (currentMillis - lastSensorRead >= 1000) {
    lastSensorRead = currentMillis;
    readSensors();
  }

  // --- 2. Heater control ---
  // handleHeater(currentMillis);

  // --- 3. pH correction control ---
  handlePH(currentMillis);

  // --- 4. Display every 1.5s ---
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
}

// Non-blocking pH read using your averaging formula
float readPH() {
  phBuffer[phIndex] = analogRead(PH_PIN);
  phIndex = (phIndex + 1) % 10;

  int sorted[10];
  memcpy(sorted, phBuffer, sizeof(sorted));
  // sort readings
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

// ---------------- CONTROL FUNCTIONS ----------------
void handleHeater(unsigned long currentMillis) {
  // Start heating if cold and enough water
  if (!heaterOn && !afterHeatingPump && temperature < DESIRED_TEMP && waterLevel > MIN_WATER_LEVEL_CM) {
    heaterOn = true;
    heaterStartTime = currentMillis;
    digitalWrite(HEATER, HIGH);
  }

  // Stop heater after 10s
  if (heaterOn && (currentMillis - heaterStartTime >= HEATER_ON_TIME)) {
    heaterOn = false;
    digitalWrite(HEATER, LOW);

    // After heating, pump water into the system
    afterHeatingPump = true;
    pumpInActive = true;
    pumpStartTime = currentMillis;
    digitalWrite(PUMP_IN, HIGH);
  }

  // Stop pump after heating
  if (afterHeatingPump && pumpInActive && (currentMillis - pumpStartTime >= PUMP_RUN_TIME)) {
    digitalWrite(PUMP_IN, LOW);
    pumpInActive = false;
    afterHeatingPump = false;
  }
}

// pH control
void handlePH(unsigned long currentMillis) {
  if ((phValue < PH_LOW || phValue > PH_HIGH) && phState == PH_IDLE && waterLevel > MIN_WATER_LEVEL_CM) {
    // Start draining
    phState = PH_DRAINING;
    phStartTime = currentMillis;
    digitalWrite(PUMP_OUT, HIGH);
  }

  // Stop draining after 5s, start refilling
  if (phState == PH_DRAINING && (currentMillis - phStartTime >= PUMP_RUN_TIME)) {
    digitalWrite(PUMP_OUT, LOW);
    phState = PH_REFILLING;
    phStartTime = currentMillis;
    digitalWrite(PUMP_IN, HIGH);
  }

  // Stop refilling after 5s
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
  lcd.print("  TDS:");
  lcd.print(tdsValue, 0);

  int waterpercentage = (waterLevel / TANK_EMPTY_CM) * 100;

  lcd.setCursor(0, 2);
  lcd.print("Lvl:");
  lcd.print(waterpercentage, 1);
  lcd.print("%");

  lcd.setCursor(0, 3);
  if (heaterOn)
    lcd.print("Heater:ON");
  else if (pumpInActive)
    lcd.print("PumpIn:ON");
  else if (phState == PH_DRAINING)
    lcd.print("Drain:ON");
  else
    lcd.print("System:OK");
}
