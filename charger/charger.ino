#include <LiquidCrystal_I2C.h>
#include <ACS712.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define VOLTAGE A1
#define CURRENT A0

ACS712 currentSensor(ACS712_05B, CURRENT);

float readVoltage(int pin);
float getSmoothedCurrent(ACS712& sensor, int samples = 20);

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();


  currentSensor.calibrate();

  lcd.print(F("Abubakar A Alkali"));
  lcd.setCursor(0, 1);
  lcd.print(F("2023/nd/ret/009"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Abubakar Ka,ilu darki"));
  lcd.setCursor(0, 1);
  lcd.print(F("2023/nd/ret/004"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Muhammad Adamu bomboy"));
  lcd.setCursor(0, 1);
  lcd.print(F("2023/nd/ret/003"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Muhammad Ibrahim Muhammad"));
  lcd.setCursor(0, 1);
  lcd.print(F("2022/nd/ret/010"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Saleh Abubakar Janda"));
  lcd.setCursor(0, 1);
  lcd.print(F("2023/nd/ret/005"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Mal.Muhammad Abubakar"));
  delay(3000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0, 0);
  lcd.print(F("Voltage: "));
  lcd.print(readVoltage(VOLTAGE));
  lcd.print(F("V   "));

  lcd.setCursor(0, 1);
  lcd.print(F("Current: "));
  lcd.print(getSmoothedCurrent(currentSensor));
  lcd.print(F("A   "));
}

float readVoltage(int pin) {
  float R1 = 30000.0;
  float R2 = 7500.0;
  float ref_voltage = 5.0;
  int adc_value = analogRead(pin);

  // Determine voltage at ADC input
  float adc_voltage = (adc_value * ref_voltage) / 1024.0;

  // Calculate voltage at divider input
  float in_voltage = adc_voltage * (R1 + R2) / R2;

  return in_voltage;
}

float getSmoothedCurrent(ACS712& sensor, int samples = 20) {
  float sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += sensor.getCurrentDC();
    delay(2);  // Small delay between samples
  }
  return max(0.0, sum / samples);  // Prevent negative currents
}
