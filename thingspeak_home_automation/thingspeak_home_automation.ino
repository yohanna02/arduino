#include <LiquidCrystal_I2C.h>
#include <ACS712.h>
#include <ZMPT101B.h>
#include <DHT.h>

#include "command.h"

#define ACTUAL_VOLTAGE 220.0f
#define TOLLERANCE 3.0f
#define MAX_TOLLERANCE_VOLTAGE (ACTUAL_VOLTAGE + TOLLERANCE)
#define MIN_TOLLERANCE_VOLTAGE (ACTUAL_VOLTAGE - TOLLERANCE)

#define FAST_STEP 5.0f
#define FINE_STEP 0.25f

#define VOLTAGE_SENSOR_PIN A0
#define CURRENT_SENSOR_PIN_1 A1

#define ESP_SERIAL Serial2

#define DHT_PIN 22
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

ACS712 current_sensor_1(ACS712_05B, CURRENT_SENSOR_PIN_1);
ZMPT101B voltageSensor(VOLTAGE_SENSOR_PIN, 50.0);

float energyConsumed = 0;
unsigned long lastEnergyUpdate = 0;

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define BUZZER 2
#define FAN 3
#define HUMIDIFIER 4
#define STORE_BULB 5
#define ROOM_BULB 6
#define SECURITY_BULB 7
#define RELAY_7 8
#define RELAY_8 9

#define TRIG 10
#define ECHO 11

#define SMOKE A2
#define PIR A3
#define LDR A4

float temp = 0;
float humd = 0;

unsigned long prevMillis = 0;

bool tempAuto = true;
unsigned long tempResetMillis = 0;

bool humidifierAuto = true;
unsigned long humdResetMillis = 0;

bool roomOn = false;

bool securityAuto = true;
unsigned long securityResetMillis = 0;

bool buzzerAuto = true;
unsigned long buzzerResetMillis = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(FAN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);
  pinMode(STORE_BULB, OUTPUT);
  pinMode(ROOM_BULB, OUTPUT);
  pinMode(SECURITY_BULB, OUTPUT);
  pinMode(RELAY_7, OUTPUT);
  pinMode(RELAY_8, OUTPUT);

  ESP_SERIAL.begin(9600);

  lcd.init();
  lcd.backlight();

  dht.begin();

  lcd.print(F("Calibrating"));
  lcd.setCursor(0, 1);
  lcd.print(F("System"));

  for (int i = 0; i < 50; i++) {
    current_sensor_1.calibrate();
    delay(100);
  }

  temp = dht.readTemperature();
  temp = dht.readHumidity();

  float sensitivity = 0.0f;
  float voltageNow;

  while (sensitivity <= 1000.0f) {
    voltageSensor.setSensitivity(sensitivity);
    voltageNow = voltageSensor.getRmsVoltage();

    if (voltageNow >= MIN_TOLLERANCE_VOLTAGE && voltageNow <= MAX_TOLLERANCE_VOLTAGE) {
      break;
    }

    sensitivity += FAST_STEP;
  }

  if (sensitivity > 1000.0f) {
    lcd.clear();
    lcd.print(F("Failed to"));
    lcd.setCursor(0, 1);
    lcd.print(F("calibrate"));
    while (true)
      ;
  }

  float fineSensitivity = sensitivity - FAST_STEP;
  if (fineSensitivity < 0) fineSensitivity = 0;

  while (fineSensitivity <= sensitivity + FAST_STEP) {
    voltageSensor.setSensitivity(fineSensitivity);
    voltageNow = voltageSensor.getRmsVoltage();

    if (voltageNow >= MIN_TOLLERANCE_VOLTAGE && voltageNow <= MAX_TOLLERANCE_VOLTAGE) {
      lcd.clear();
      return;
    }

    fineSensitivity += FINE_STEP;
  }

  lcd.clear();
  lcd.print(F("Failed to"));
  lcd.setCursor(0, 1);
  lcd.print(F("calibrate"));
  while (true)
    ;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - prevMillis > 2000) {
    temp = dht.readTemperature();
    humd = dht.readHumidity();

    sendCommand(ENERGY, energyConsumed);

    if (roomOn) {
      digitalWrite(ROOM_BULB, HIGH);
    } else {
      digitalWrite(ROOM_BULB, LOW);
    }

    prevMillis = millis();
  }

  if (ESP_SERIAL.available()) {
    String command = ESP_SERIAL.readStringUntil('\n');

    processCommand(command);
  }

  switchToAuto();

  if (tempAuto) {
    if (temp >= 24.0) {
      digitalWrite(FAN, HIGH);
    } else if (temp <= 22.0) {
      digitalWrite(FAN, LOW);
    }
  }

  if (humidifierAuto) {
    if (humd < 40) {
      digitalWrite(HUMIDIFIER, HIGH);
    } else if (humd >= 45) {
      digitalWrite(HUMIDIFIER, LOW);
    }
  }

  if (securityAuto) {
    int lightValue = analogRead(LDR);

    if (lightValue > 10) {
      digitalWrite(SECURITY_BULB, LOW);
    } else {
      digitalWrite(SECURITY_BULB, HIGH);
    }
  }

  if (buzzerAuto) {
    if (digitalRead(PIR) || analogRead(SMOKE) > 500) {
      digitalWrite(BUZZER, HIGH);
      delay(3000);
    } else {
      digitalWrite(BUZZER, LOW);
    }
  }

  int distance = getDistance(TRIG, ECHO);

  if (distance >= 4 && distance < 7) {
    lcd.clear();
    lcd.print(F("Presense detected"));
    lcd.setCursor(0, 1);
    lcd.print("Turing ");
    lcd.print(roomOn ? "off" : "on");
    lcd.print(F(" light"));
    roomOn = !roomOn;
    delay(1000);
    lcd.clear();
  }

  float Vrms = voltageSensor.getRmsVoltage();
  float Irms = readCurrentWithCheck(current_sensor_1);

  float realPower = Vrms * Irms;
  unsigned long now = millis();
  float deltaTimeHours = (now - lastEnergyUpdate) / 3600000.0;
  lastEnergyUpdate = now;
  float energyThisLoop = realPower * deltaTimeHours;

  energyConsumed += energyThisLoop;

  lcd.setCursor(0, 0);
  lcd.print(F("Temp: "));
  lcd.print(temp);
  lcd.print(F(" C   "));

  // lcd.setCursor(0, 0);
  // lcd.print(F("Distance: "));
  // lcd.print(distance);
  // lcd.print(F("    "));

  lcd.setCursor(0, 1);
  lcd.print(F("Humd: "));
  lcd.print(humd);
  lcd.print(F(" %"));

  // lcd.setCursor(0, 1);
  // lcd.print(F("PIR: "));
  // lcd.print(digitalRead(PIR));
  // lcd.print(F(" "));

  // lcd.setCursor(0, 2);
  // lcd.print(F("Gas "));
  // lcd.print(analogRead(SMOKE));
  // lcd.print(F("   "));

  lcd.setCursor(0, 2);
  lcd.print(F("V: "));
  lcd.print(Vrms);
  lcd.print(F(", I:"));
  lcd.print(Irms);
  lcd.print(F("   "));


  lcd.setCursor(0, 3);
  lcd.print(F("Energy: "));
  lcd.print(energyConsumed);
  lcd.print(F(" W/h    "));
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
    if (readings[i] <= 0.15) {
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

void sendCommand(String key, float value) {
  ESP_SERIAL.print(key);
  ESP_SERIAL.print("=");
  ESP_SERIAL.print(value, 2);  // send with 2 decimal precision
  ESP_SERIAL.print('\n');
}

void sendCommand(String key, String value) {
  ESP_SERIAL.print(key);
  ESP_SERIAL.print("=");
  ESP_SERIAL.print(value);
  ESP_SERIAL.print('\n');
}

void sendCommand(String key) {
  ESP_SERIAL.print(key);
  ESP_SERIAL.print('\n');
}

void processCommand(String cmd) {
  int sepIndex = cmd.indexOf('=');

  if (sepIndex != -1) {
    String key = cmd.substring(0, sepIndex);
    float value = cmd.substring(sepIndex + 1).toFloat();


  } else {
    if (cmd == FAN_ON) {
      digitalWrite(FAN, HIGH);
      tempAuto = false;
      tempResetMillis = millis();
    } else if (cmd == FAN_OFF) {
      digitalWrite(FAN, LOW);
      tempAuto = false;
      tempResetMillis = millis();
    } else if (cmd == HUMIDIFIER_ON) {
      digitalWrite(HUMIDIFIER, HIGH);
      humidifierAuto = false;
      humdResetMillis = millis();
    } else if (cmd == HUMIDIFIER_OFF) {
      digitalWrite(HUMIDIFIER, LOW);
      humidifierAuto = false;
      humdResetMillis = millis();
    } else if (cmd == ROOM_ON) {
      roomOn = true;
    } else if (cmd == ROOM_OFF) {
      roomOn = false;
    } else if (cmd == SECURITY_ON) {
      digitalWrite(SECURITY_BULB, HIGH);
      securityAuto = false;
      securityResetMillis = millis();
    } else if (cmd == SECURITY_OFF) {
      digitalWrite(SECURITY_BULB, LOW);
      securityAuto = false;
      securityResetMillis = millis();
    } else if (cmd == SOCKET_ON) {
      digitalWrite(RELAY_7, HIGH);
      digitalWrite(RELAY_8, HIGH);
    } else if (cmd == SOCKET_OFF) {
      digitalWrite(RELAY_7, LOW);
      digitalWrite(RELAY_8, LOW);
    } else if (cmd == STORE_ON) {
      digitalWrite(STORE_BULB, HIGH);
    } else if (cmd == STORE_OFF) {
      digitalWrite(STORE_BULB, LOW);
    } else if (cmd == BUZZER_ON) {
      digitalWrite(BUZZER, HIGH);
      buzzerAuto = false;
      buzzerResetMillis = millis();
    } else if (cmd == BUZZER_OFF) {
      digitalWrite(BUZZER, LOW);
      buzzerAuto = false;
      buzzerResetMillis = millis();
    }
  }
}

void switchToAuto() {
  unsigned long currentMillis = millis();

  if (currentMillis - tempResetMillis > 60000) {
    tempAuto = true;
  }

  if (currentMillis - humdResetMillis > 60000) {
    humidifierAuto = true;
  }

  if (currentMillis - securityResetMillis > 60000) {
    securityAuto = true;
  }

  if (currentMillis - buzzerResetMillis > 60000) {
    buzzerAuto = true;
  }
}

int getDistance(int trigPin, int echoPin) {
  long duration;
  int distance;

  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a 10-microsecond pulse to trigPin
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin and calculate the distance
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;  // Convert time to distance (cm)

  return distance;  // Return the distance value
}
