#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

SoftwareSerial _serial_2(3, 2);
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define ESP_SERIAL _serial_2
#define GSM_SERIAL Serial

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 5

#define BUZZER 6

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

#define TRIG_PIN 8
#define ECHO_PIN 9

float tempC = 0;
int fuelLevel = 0;
float voltage = 0;
float current = 0;

#define VOLTAGE_SENSOR_PIN A2
#define CURRENT_SENSOR_PIN A1

float calibration_factor = 0.2;

const float sensitivity = 0.100;  // 185mV/A for ACS712-05B
const float voltageOffset = 2.5;  // No current = 2.5V (midpoint)
#define SAMPLES 100               // Number of readings for accuracy

unsigned long sendMillis = 0;

#define NUMBER "+2348038048487"
bool sent = false;

void setup() {
  // put your setup code here, to run once:
  ESP_SERIAL.begin(9600);
  GSM_SERIAL.begin(9600);
  pinMode(BUZZER, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(VOLTAGE_SENSOR_PIN, INPUT);
  pinMode(CURRENT_SENSOR_PIN, INPUT);
  sensors.begin();
  lcd.init();
  lcd.backlight();
  delay(1000);
  lcd.clear();

  // gsm_init();

  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);

  int distance = getDistance(TRIG_PIN, ECHO_PIN);
  distance = map(distance, 4, 23, 100, 0);
  fuelLevel = constrain(distance, 0, 100);
  // fuelLevel = distance;

  voltage = getVoltage();

  current = getCurrent();

  lcd.setCursor(0, 0);
  lcd.print(F("Temp: "));
  lcd.print(tempC);
  lcd.print(F(" C     "));

  lcd.setCursor(0, 1);
  lcd.print(F("Fuel Level: "));
  lcd.print(fuelLevel);
  lcd.print(F("%     "));

  lcd.setCursor(0, 2);
  lcd.print(F("Voltage: "));
  lcd.print(voltage);
  lcd.print(F(" V    "));

  lcd.setCursor(0, 3);
  lcd.print(F("Current: "));
  lcd.print(current);
  lcd.print(F(" A    "));

  if (fuelLevel < 30 || tempC > 40) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  if (millis() - sendMillis > 20000) {
    send_data();
    sendMillis = millis();
  }

  if (fuelLevel < 40 && !sent) {
    lcd.clear();
    lcd.print(F("Fuel Level Low"));
    send_sms("Fuel Level LOW", NUMBER);
    delay(3000);
    lcd.clear();
    sent = true;
  } else if (fuelLevel >= 40) {
    sent = false;
  }
}

float getVoltage() {
  int rawValue = analogRead(VOLTAGE_SENSOR_PIN);
  float voltage = (rawValue * 5.0 / 1023.0) * calibration_factor;

  return voltage;
}

float getCurrent() {
  float sum = 0.0;

  for (int i = 0; i < SAMPLES; i++) {
    int rawValue = analogRead(CURRENT_SENSOR_PIN);
    float voltage = rawValue * (5.0 / 1023.0);
    float current = (voltage - voltageOffset) / sensitivity;
    sum += current * current;  // Squaring each current reading
    delay(1);
  }

  float rmsCurrent = sqrt(sum / SAMPLES);  // Calculate RMS

  return rmsCurrent;
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

void send_data() {
  lcd.clear();
  lcd.print(F("Sending data"));
  String sendData = String(tempC) + ":" + String(fuelLevel) + ":" + String(voltage) + ":" + String(current);
  ESP_SERIAL.print(sendData);
  delay(2000);
  lcd.clear();
}

void send_sms(const char *sms_data, const char *number) {
  GSM_SERIAL.println("AT+CMGF=1");
  delay(1000);
  char num_data[26];
  sprintf(num_data, "AT+CMGS=\"%s\"", number);
  GSM_SERIAL.println(num_data);
  delay(1000);
  GSM_SERIAL.print(sms_data);
  delay(1000);
  GSM_SERIAL.write(26);
  delay(1000);
}

void gsm_init() {
  lcd.clear();
  lcd.print(F("Finding Module.."));
  boolean at_flag = 1;
  while (at_flag) {
    GSM_SERIAL.println("AT");
    while (GSM_SERIAL.available() > 0) {
      if (GSM_SERIAL.find("OK"))
        at_flag = 0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print(F("Module Connected.."));
  delay(500);
  lcd.clear();
  // lcd.print("Disabling ECHO");
  bool echo_flag = 1;
  while (echo_flag) {
    GSM_SERIAL.println("ATE0");
    while (GSM_SERIAL.available() > 0) {
      if (GSM_SERIAL.find("OK"))
        echo_flag = 0;
    }
    delay(1000);
  }
  // lcd.clear();
  //  lcd.print("Echo OFF");
  delay(500);
  lcd.clear();
  lcd.print("Finding Network..");
  bool net_flag = 1;
  while (net_flag) {
    GSM_SERIAL.println("AT+CPIN?");
    while (GSM_SERIAL.available() > 0) {
      if (GSM_SERIAL.find("+CPIN: READY"))
        net_flag = 0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print("Network Found..");
  delay(500);
  lcd.clear();
}
