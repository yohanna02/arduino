#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // adjust address if needed (0x27 or 0x3F)

#define sim800 Serial  // alias so your provided sim800-based function works with hardware Serial

// === Hardware pins ===
const byte LDR_PINS[4] = { A0, A1, A2, A3 };
const byte BUZZER_PIN = 2;

// === Detection tuning ===
const float sensitivity = 0.30;     // 30% change from baseline triggers
const uint8_t consecReads = 4;      // consecutive readings required to confirm
const uint16_t calibMillis = 2000;  // calibration duration
const uint16_t sampleDelayMs = 40;  // between analog samples

// === SMS settings ===
const char PHONE_NUMBER[] = "+2347011355614";         // <- change to recipient number
const char SMS_TEXT[] = "ALERT: Intruder detected!";  // message content

// === Internal state ===
float baseline[4];
uint8_t triggerCounter[4];
bool sensorTripped[4];
bool alarmLatched = false;
bool smsSentForAlarm = false;

// Forward declarations
void calibrateSensors();
float readSensorAvg(uint8_t pin, uint8_t samples);
bool detectTrigger(uint8_t idx, float reading);
void triggerAlarm(bool on, uint8_t beamIndex);
void sendSMS(String message);  // uses sim800 (alias of Serial)

void setup() {
  // Initialize LCD
  lcd.init();
  lcd.backlight();

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("implementation"));
  delay(3000);
  lcd.clear();

  lcd.print(F("of virtual"));
  lcd.setCursor(0, 1);
  lcd.print(F("fencing with"));
  delay(3000);
  lcd.clear();

  lcd.print(F("sms"));
  lcd.setCursor(0, 1);
  lcd.print(F("alert"));
  delay(3000);
  lcd.clear();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibrating...");

  // Pins
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // SIM800L setup (hardware Serial)
  sim800.begin(9600);
  delay(1200);

  calibrateSensors();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Active");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring...");
  delay(800);
}

void loop() {
  if (!alarmLatched) {
    for (uint8_t i = 0; i < 4; ++i) {
      float val = readSensorAvg(LDR_PINS[i], 3);
      if (detectTrigger(i, val)) {
        sensorTripped[i] = true;
        alarmLatched = true;
        triggerAlarm(true, i);

        // Send SMS immediately (no retry)
        if (!smsSentForAlarm) {
          sendSMS(String(SMS_TEXT));
          smsSentForAlarm = true;  // we send only once, no retry
        }
        delay(5000);
        alarmLatched = false;
        digitalWrite(BUZZER_PIN, LOW);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("System Active");
        lcd.setCursor(0, 1);
        lcd.print("Monitoring...");
        break;
      }
    }
  }

  delay(sampleDelayMs);
}

// ------------ sensor helpers ------------
void calibrateSensors() {
  unsigned long start = millis();
  long sums[4] = { 0, 0, 0, 0 };
  int counts = 0;

  while (millis() - start < calibMillis) {
    for (uint8_t i = 0; i < 4; ++i) {
      sums[i] += analogRead(LDR_PINS[i]);
    }
    counts++;
    delay(15);
  }
  for (uint8_t i = 0; i < 4; ++i) {
    baseline[i] = sums[i] / (float)counts;
    if (baseline[i] < 1.0) baseline[i] = 1.0;
  }
}

float readSensorAvg(uint8_t pin, uint8_t samples) {
  long s = 0;
  for (uint8_t k = 0; k < samples; ++k) {
    s += analogRead(pin);
    delay(5);
  }
  return s / (float)samples;
}

bool detectTrigger(uint8_t idx, float reading) {
  float diff = fabs(reading - baseline[idx]);
  float thresh = baseline[idx] * sensitivity;

  if (diff > thresh) {
    if (triggerCounter[idx] < consecReads) triggerCounter[idx]++;
  } else {
    if (triggerCounter[idx] > 0) triggerCounter[idx]--;
  }

  if (triggerCounter[idx] >= consecReads) {
    triggerCounter[idx] = consecReads;
    return true;
  }
  return false;
}

// ------------ alarm / SMS ------------
void triggerAlarm(bool on, uint8_t beamIndex) {
  if (on) {
    digitalWrite(BUZZER_PIN, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!!! INTRUDER !!!");
    lcd.setCursor(0, 1);
    lcd.print("Beam ");
    lcd.print(beamIndex + 1);
    lcd.print(" Broken");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Active");
    lcd.setCursor(0, 1);
    lcd.print("Monitoring...");
  }
}

// Uses the exact style you provided but sends only to the single PHONE_NUMBER.
// We alias sim800 to Serial so your SIM800L (on default serial) is used.
void sendSMS(String message) {
  // Set SMS text mode
  sim800.println("AT+CMGF=1");
  delay(500);

  // Send to the configured single number
  sim800.print("AT+CMGS=\"");
  sim800.print(PHONE_NUMBER);
  sim800.println("\"");
  delay(500);

  sim800.print(message);
  sim800.write(26);  // Ctrl+Z to send SMS
  delay(1000);
}
