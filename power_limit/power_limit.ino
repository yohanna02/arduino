#include <ACS712.h>
#include <ZMPT101B.h>
#include <LiquidCrystal_I2C.h>


#define VOLTAGE_SENSOR_PIN A1
#define CURRENT_SENSOR_PIN A0

#define RELAY_PIN 6
LiquidCrystal_I2C lcd(0x27, 16, 2);

ACS712 current_sensor(ACS712_05B, CURRENT_SENSOR_PIN);

void setup() {
  // put your setup code here, to run once:
  pinMode(RELAY_PIN, OUTPUT);
  lcd.init();
  lcd.backlight();


  lcd.print(F("Calibrating"));
  lcd.setCursor(0, 1);
  lcd.print(F("System"));
  delay(3000);
  lcd.clear();

  current_sensor.calibrate();

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("construction"));
  delay(3000);
  lcd.clear();

  lcd.print(F("of an overload"));
  lcd.setCursor(0, 1);
  lcd.print(F("protection for"));
  delay(3000);
  lcd.clear();

  lcd.print(F("transformer"));
  lcd.setCursor(0, 1);
  lcd.print(F("system"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Rilwan Yakubu"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/150809"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Tamia Mansiur Animba"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/150714"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Williams Yusuf"));
  lcd.setCursor(0, 1);
  lcd.print(F("23/151253"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr.Dr.Ahmad M.Y Jumba"));
  delay(3000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:

  float Vrms = 225;
  float Irms = readCurrentWithCheck(current_sensor);
  float realPower = Vrms * Irms;

  lcd.setCursor(0, 0);
  lcd.print(F("Power: "));
  lcd.print(realPower);
  lcd.print(F("W     "));
  lcd.setCursor(0, 1);

  if (realPower < 600) {
    lcd.print(F("System Normal        "));
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    lcd.print(F("System Overload      "));
    digitalWrite(RELAY_PIN, LOW);
  }
}

float readCurrentWithCheck(ACS712 &sensor) {
  const int NUM_READINGS = 10;
  float readings[NUM_READINGS];

  // Take 10 readings
  for (int i = 0; i < NUM_READINGS; i++) {
    readings[i] = sensor.getCurrentAC();
    delay(10);  // Small delay between readings
  }

  // Check if any reading is close to zero
  for (int i = 0; i < NUM_READINGS; i++) {
    if (readings[i] <= 0.12) {
      return 0;
    }
  }

  // Calculate average of readings
  float sum = 0;
  for (int i = 0; i < NUM_READINGS; i++) {
    sum += readings[i];
  }
  return sum / NUM_READINGS;
}
