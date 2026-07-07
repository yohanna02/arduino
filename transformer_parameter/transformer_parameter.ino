#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESPComm.h>

ESPComm esp(Serial);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ultrasonic sensor pins
const int trigPin = 6;
const int echoPin = 7;

// DS18B20 setup
const int tempPin = 2;  // data pin of DS18B20
OneWire oneWire(tempPin);
DallasTemperature sensors(&oneWire);

// Tank parameters (in cm)
const float tankHeight = 20.0;  // height of oil tank
const float minOilLevel = 2.0;  // distance from sensor to oil when full

void setup() {
  esp.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  sensors.begin();
  randomSeed(analogRead(A2));  // seed random generator

  /* Design and construction of base station parameter monitoring and reporting system
1. Ochiwu Daniel ogigbo 
BPU/EEE/H/EEP/23/10028
2. Tertsea Aondosoo Solomon
SUPERVISOR: ENGR. Agbo Victor */

  lcd.setCursor(0, 0);
  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("construction"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("of base station"));
  lcd.setCursor(0, 1);
  lcd.print(F("parameter monitoring"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("and reporting"));
  lcd.setCursor(0, 1);
  lcd.print(F("system"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Khalid A.Ghaji"));
  lcd.setCursor(0, 1);
  lcd.print(F("HND/EE/PM/010"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Isah ilyasu"));
  lcd.setCursor(0, 1);
  lcd.print(F("HND/EE/PM/017"));
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Abubakar Hamisu"));
  delay(3000);
  lcd.clear();
}

void loop() {
  // --- Simulated Voltage and Current ---
  float voltage = random(2100, 2400) / 10.0;  // 210–240 V
  float current = random(0, 500) / 1000.0;    // 0.00–0.50 A

  // --- Oil Level Measurement (Ultrasonic) ---
  float distance = getDistance();
  if (distance < minOilLevel) distance = minOilLevel;
  if (distance > tankHeight) distance = tankHeight;

  float oilPercent = ((tankHeight - distance) / (tankHeight - minOilLevel)) * 100.0;
  oilPercent = constrain(oilPercent, 0, 100);

  // --- Temperature Measurement (DS18B20) ---
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);  // in °C

  // --- Display Data on LCD ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltage, 1);
  lcd.print("V ");
  lcd.print("I:");
  lcd.print(current, 3);
  lcd.print("A");

  lcd.setCursor(0, 1);
  lcd.print("Oil:");
  lcd.print(oilPercent, 0);
  lcd.print("% ");

  if (oilPercent < 30) {
    lcd.print("LOW ");
  } else {
    lcd.print("OK ");
  }

  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  if (temperature == DEVICE_DISCONNECTED_C) {
    lcd.print("Err");
  } else {
    lcd.print(temperature, 1);
    lcd.print((char)223);  // degree symbol
    lcd.print("C");
  }
  delay(2000);

  esp.send("voltage", voltage);
  delay(100);
  esp.send("current", current);
  delay(100);
  esp.send("temp", temperature);
  delay(100);
  esp.send("oil", oilPercent);
}

// --- Function to measure distance using ultrasonic sensor ---
float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distanceCm = duration * 0.0343 / 2;  // convert to cm
  return distanceCm;
}
