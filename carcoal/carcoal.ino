#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "max6675.h"
#include "HX711.h"

#define COMMUNICATION Serial
#define calibration_factor -109.4 //-74.4

#define LOADCELL_DOUT_PIN  2
#define LOADCELL_SCK_PIN  3

#define PWM_PIN 10
#define POT_PIN A0
#define RELAY_PIN 8
#define RELAY_PWM 9

LiquidCrystal_I2C lcd(0x27, 16, 2);
HX711 scale;

unsigned long prevMillis = 0;
long interval = 1000;

bool shareScreen = false;
int pwm_val = 0,
    mapped_pwm = 0;
float temperature = 0,
      read_load_cell = 0;

int thermoDO = 4,
    thermoCS = 5,
    thermoCLK = 6;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

enum class fanEnum { ON, OFF };
fanEnum isOn = fanEnum::OFF;

void setup() {
  COMMUNICATION.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(PWM_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PWM, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(RELAY_PWM, LOW);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  //  scale.set_scale();
  //  scale.tare();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System is ready      ");
  lcd.setCursor(0, 1);
  lcd.print("******     ");
  // wait for MAX chip to stabilize
  delay(2000);
}

void loop() {
  unsigned long current = millis();
  if (current - prevMillis > interval) {
    prevMillis = current;
    temperature = thermocouple.readCelsius();
    int read_pot = analogRead(POT_PIN);
    pwm_val = ((read_pot <= 20 || read_pot >= 1000) ? (read_pot + 50) : read_pot);
    if (pwm_val > 1024) pwm_val = 1024;
    else if (pwm_val < 0) pwm_val = 0;
    pwm_val = map(pwm_val, 0, 1024, 260, 0);
    mapped_pwm = map(pwm_val, 0, 255, 0, 100);
    isOn = (int(pwm_val) <= 10) ? fanEnum::OFF : fanEnum::ON;
  }

  digitalWrite(RELAY_PWM, isOn == fanEnum::ON  ? HIGH : LOW);
  analogWrite(PWM_PIN, int(pwm_val));
  //  digitalWrite(RELAY_PWM, HIGH);
  //  analogWrite(PWM_PIN, 45);

  if (scale.is_ready()) {
    float readLoadCell = float(abs(scale.get_units(10) * 2)); // Average over 10 readings
    float total_weight = (float(readLoadCell) * 2); // Compensate for half the pan weight
    read_load_cell = total_weight;
    delay(200);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("HX711 not ready.");
    return 0;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PWM ");
  lcd.print(String(mapped_pwm) + "%, " + String(temperature) + "*C                  ");
  lcd.setCursor(0, 1);
  lcd.print("Weight ");
  lcd.print(read_load_cell, 1);
  lcd.print("g                            ");
  delay(1000);
}