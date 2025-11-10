#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Adafruit_MLX90614.h>
#include <MAX30100_PulseOximeter.h>
#include <HX710AB.h>

// Pin Definitions
#define SERVO_PIN 9
#define ECG_PIN A0
#define ECG_LO_PLUS 12
#define ECG_LO_MINUS 13
#define HX_DATA_PIN 10
#define HX_CLOCK_PIN 11

// Objects
Servo ventServo;
LiquidCrystal_I2C lcd(0x27, 20, 4);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
PulseOximeter pox;
HX710A hx(HX_DATA_PIN, HX_CLOCK_PIN);

// Keypad
const byte ROWS = 4, COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {8, 7, 6, 5};
byte colPins[COLS] = {4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Modes
enum class Sys_Mode { SELECT, VENTILATOR, MONITOR };
Sys_Mode currentMode = Sys_Mode::SELECT;

// Ventilator Parameters
int breathsPerMinute = 12;
int tidalVolumeAngle = 90;
unsigned long breathInterval;
unsigned long lastBreathTime = 0;

enum InputMode { NONE, BPM, VOLUME };
InputMode inputMode = NONE;
String inputBuffer = "";

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  ventServo.attach(SERVO_PIN);

  pinMode(ECG_LO_PLUS, INPUT);
  pinMode(ECG_LO_MINUS, INPUT);

  hx.begin();

  if (!mlx.begin()) {
    lcd.print("MLX90614 ERR");
    while (1);
  }

  if (!pox.begin()) {
    lcd.print("MAX30100 ERR");
    while (1);
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  updateBreathInterval();
  showModeSelection();
}

void loop() {
  char key = keypad.getKey();
  if (key) handleKeyInput(key);

  if (currentMode == Sys_Mode::VENTILATOR) {
    unsigned long currentTime = millis();
    if (currentTime - lastBreathTime >= breathInterval) {
      simulateBreath();
      lastBreathTime = currentTime;
    }
    displaySensorData(); // Still show vitals
  } else if (currentMode == Sys_Mode::MONITOR) {
    displaySensorData();
  }
}

// ===================== Mode Handlers ======================

void showModeSelection() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select Mode:");
  lcd.setCursor(0, 1);
  lcd.print("1: Ventilator");
  lcd.setCursor(0, 2);
  lcd.print("2: Monitor");
}

void handleKeyInput(char key) {
  if (currentMode == Sys_Mode::SELECT) {
    if (key == '1') {
      currentMode = Sys_Mode::VENTILATOR;
      lcd.clear();
      lcd.print("Ventilator Mode");
      delay(1000);
      showMainScreen();
    } else if (key == '2') {
      currentMode = Sys_Mode::MONITOR;
      lcd.clear();
      lcd.print("Monitoring Mode");
      delay(1000);
    }
  } else {
    if (key == '*') {
      inputMode = BPM;
      inputBuffer = "";
      lcd.clear();
      lcd.print("Set BPM (5-40):");
    } else if (key == '9') {
      inputMode = VOLUME;
      inputBuffer = "";
      lcd.clear();
      lcd.print("Set Vol (30-150):");
    } else if (key == '#') {
      int value = inputBuffer.toInt();
      if (inputMode == BPM && value >= 5 && value <= 40) {
        breathsPerMinute = value;
        updateBreathInterval();
      } else if (inputMode == VOLUME && value >= 30 && value <= 150) {
        tidalVolumeAngle = value;
      }
      inputMode = NONE;
      showMainScreen();
    } else if (isDigit(key) && inputMode != NONE) {
      inputBuffer += key;
      lcd.setCursor(0, 1);
      lcd.print("Val: ");
      lcd.print(inputBuffer);
    }
  }
}

// ===================== Ventilation Logic ======================

void simulateBreath() {
  ventServo.write(tidalVolumeAngle);
  delay(800);
  ventServo.write(0);
  delay(200);
}

void updateBreathInterval() {
  breathInterval = 60000UL / breathsPerMinute;
}

void showMainScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BPM:");
  lcd.print(breathsPerMinute);
  lcd.print(" Vol:");
  lcd.print(tidalVolumeAngle);
}

// ===================== Sensor Display ======================

void displaySensorData() {
  pox.update();
  float heartRate = pox.getHeartRate();
  float spo2 = pox.getSpO2();
  float temp = mlx.readObjectTempC();
  int32_t pressure = hx.read(false);
  String ecgStatus;

  if ((digitalRead(ECG_LO_PLUS) == 1) || (digitalRead(ECG_LO_MINUS) == 1)) {
    ecgStatus = "Leads Off";
  } else {
    ecgStatus = String(analogRead(ECG_PIN));
  }

  lcd.setCursor(0, 1);
  lcd.print("HR:");
  lcd.print(heartRate, 0);
  lcd.print(" SpO2:");
  lcd.print(spo2, 0);

  lcd.setCursor(0, 2);
  lcd.print("T:");
  lcd.print(temp, 1);
  lcd.print(" P:");
  lcd.print(pressure);

  lcd.setCursor(0, 3);
  lcd.print("ECG:");
  lcd.print(ecgStatus);
}
