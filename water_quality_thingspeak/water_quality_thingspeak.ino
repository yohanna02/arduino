#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PH_PIN 36         // GPIO36
#define TURBIDITY_PIN 39  // GPIO39
#define TDS_PIN 34        // GPIO34

// LCD I2C address and size
LiquidCrystal_I2C lcd(0x27, 16, 4);  // Address 0x27 is typical, adjust if needed

void setup() {
  // Serial.begin(115200);

  // LCD Init
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sensor Monitoring");

  delay(1000);
}

void loop() {
  // === Read Sensors ===
  int phRaw = analogRead(PH_PIN);
  int turbidityRaw = analogRead(TURBIDITY_PIN);
  int tdsRaw = analogRead(TDS_PIN);

  // === Convert Readings ===
  float phVoltage = (phRaw / 4095.0) * 3.3;  // ESP32 is 12-bit ADC, 3.3V
  float turbVoltage = (turbidityRaw / 4095.0) * 3.3;
  float tdsVoltage = (tdsRaw / 4095.0) * 3.3;

  // --- pH Calculation ---
  float phValue = -6.0 * phVoltage + 22.0;  // Calibrate based on your probe

  // --- Turbidity Calculation ---
  float turbidityNTU = map(turbidityRaw, 0, 4095, 3000, 0);  // Rough estimate

  // --- TDS Calculation ---
  float tdsValue = (133.42 * pow(tdsVoltage, 3) - 255.86 * pow(tdsVoltage, 2) + 857.39 * tdsVoltage) * 0.5;

  // === Print to Serial ===
  // Serial.print("pH: ");
  // Serial.print(phValue, 2);
  // Serial.print(" | Turbidity: ");
  // Serial.print(turbidityNTU);
  // Serial.print(" NTU");
  // Serial.print(" | TDS: ");
  // Serial.print(tdsValue, 1);
  // Serial.println(" ppm");

  // === Display on LCD ===
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("pH: ");
  lcd.print(phValue, 2);

  lcd.setCursor(0, 1);
  lcd.print("Turb: ");
  lcd.print(turbidityNTU);
  lcd.print(" NTU");

  lcd.setCursor(0, 2);
  lcd.print("TDS: ");
  lcd.print(tdsValue, 1);
  lcd.print(" ppm");

  delay(2000);
}
