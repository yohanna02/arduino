#include <LiquidCrystal_I2C.h>
#include <ACS712.h>
#include <HardwareSerial.h>

#define RELAY_PIN 25   // Relay control pin (change as needed)
#define LOCKED_LED 26  // Optional LED to indicate locked state

// SIM800 setup
HardwareSerial sim800(1);  // Use UART1 on ESP32
#define SIM800_TX 17
#define SIM800_RX 16

// ACS712 current sensors (5A models)
#define SENSOR_MAIN 34
#define SENSOR_BYPASS 35

ACS712 mainSensor(SENSOR_MAIN, 5.0, 1023, 185);  // 185 mV/A for 5A module
ACS712 bypassSensor(SENSOR_BYPASS, 5.0, 1023, 185);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Power line voltage (adjust if different)
#define LINE_VOLTAGE 220.0

// Thresholds
#define BYPASS_DIFF_THRESHOLD 0.3  // Amperes difference to detect bypass
#define LOCK_DELAY 2000            // Time before locking after detection

bool isLocked = false;

void setup() {
  Serial.begin(115200);
  sim800.begin(9600, SERIAL_8N1, SIM800_RX, SIM800_TX);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Meter Init");
  delay(2000);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LOCKED_LED, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);  // Relay ON (system active)
  digitalWrite(LOCKED_LED, LOW);

  mainSensor.calibrate();
  bypassSensor.calibrate();

  lcd.clear();
}

void loop() {
  if (!isLocked) {
    float mainCurrent = abs(mainSensor.getCurrentAC());
    float bypassCurrent = abs(bypassSensor.getCurrentAC());

    // Calculate power consumption (Watts)
    float power = mainCurrent * LINE_VOLTAGE;

    lcd.setCursor(0, 0);
    lcd.print("Power: ");
    lcd.print(power, 1);
    lcd.print(" W   ");

    lcd.setCursor(0, 1);
    lcd.print("Main:");
    lcd.print(mainCurrent, 2);
    lcd.print("A  ");

    lcd.print("Byp:");
    lcd.print(bypassCurrent, 2);
    lcd.print("A");

    // Check for bypass tampering
    if ((bypassCurrent - mainCurrent) > BYPASS_DIFF_THRESHOLD) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bypass Detected!");
      lcd.setCursor(0, 1);
      lcd.print("Locking...");

      delay(LOCK_DELAY);
      lockMeter();
    }

    delay(1000);
  }
}

// ==============================
// Lock the meter
// ==============================
void lockMeter() {
  isLocked = true;
  digitalWrite(RELAY_PIN, LOW);  // Turn relay OFF
  digitalWrite(LOCKED_LED, HIGH);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Meter LOCKED!");
  lcd.setCursor(0, 1);
  lcd.print("OTP sent via SMS");

  String otp = generateOTP(4);
  String message = "Tampering detected! Meter locked. OTP: " + otp;
  sendSMS(message);
}

// ==============================
// Send SMS via SIM800
// ==============================
void sendSMS(String message) {
  sim800.println("AT+CMGF=1");
  delay(500);
  sim800.println("AT+CMGS=\"+2347080948252\"");  // Change to your phone number
  delay(500);
  sim800.print(message);
  sim800.write(26);  // Ctrl+Z
  delay(3000);
}

// ==============================
// Generate random OTP
// ==============================
String generateOTP(int length) {
  String otp = "";
  for (int i = 0; i < length; i++) {
    otp += String(random(0, 10));
  }
  return otp;
}
