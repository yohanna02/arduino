/*Development of a functional cloud chamber using Microcontroller
Omojesu Josephine Bolaji
Reg number: 23/150245
Topic: Design and Construction of a Cloud Chamber
Supervisor: Mr. Buba Dauda */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// === LCD setup ===
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust address if needed

// === DHT setup ===
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// === Pin definitions ===
const int potPin = A0;
const int buttonPin = 3;
const int relayPin = 11;

// === Relay and button state ===
bool relayState = false;
// bool lastButtonState = HIGH;
// unsigned long lastDebounceTime = 0;
// const unsigned long debounceDelay = 50;

// === Non-blocking timing ===
unsigned long lastSensorUpdate = 0;
const unsigned long sensorInterval = 2000; // 1 second

void setup() {
  lcd.init();
  lcd.backlight();

  dht.begin();

  pinMode(potPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Button to GND
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Relay off

  lcd.setCursor(0, 0);
  lcd.print("Development of");
  lcd.setCursor(0, 1);
  lcd.print("a functional cloud");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("chamber using");
  lcd.setCursor(0, 1);
  lcd.print("Microcontroller");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Omojesu J.Bolaji");
  lcd.setCursor(0, 1);
  lcd.print("23/150245");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Supervised by");
  lcd.setCursor(0, 1);
  lcd.print("Mr.Buba Dauda");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("System Starting...");
  delay(2000);
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();

  // === Handle button (non-blocking with debounce) ===
  bool reading = digitalRead(buttonPin);
  // if (reading != lastButtonState) {
  //   lastDebounceTime = currentMillis;
  // }

  // if ((currentMillis - lastDebounceTime) > debounceDelay) {
  //   if (lastButtonState == HIGH && reading == LOW) {
  //     relayState = !relayState;
  //     digitalWrite(relayPin, relayState ? HIGH : LOW);
  //   }
  // }
  // lastButtonState = reading;

  relayState = reading == LOW ? true : false;
  digitalWrite(relayPin, relayState);

  // === Sensor reading and LCD update every 1 second ===
  if (currentMillis - lastSensorUpdate >= sensorInterval) {
    lastSensorUpdate = currentMillis;

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    int potValue = analogRead(potPin);
    int voltagePercent = map(potValue, 0, 1023, 0, 100);

    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(isnan(temp) ? 0 : temp);
    lcd.print("C H:");
    lcd.print(isnan(hum) ? 0 : hum);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("V:");
    lcd.print(voltagePercent);
    lcd.print("% R:");
    lcd.print(relayState ? "ON " : "OFF");
  }
}
