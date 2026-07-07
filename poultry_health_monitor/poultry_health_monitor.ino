#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SoftwareSerial.h>

// Pin Definitions
#define DHT_PIN 2
#define GAS_SENSOR_PIN A0
#define BUZZER_PIN 4
#define SMS_TX 6
#define SMS_RX 7

// DHT Sensor Type
#define DHT_TYPE DHT22

// Initialize components
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Address 0x27, 16 chars, 2 lines (change to 0x3F if 0x27 doesn't work)
DHT dht(DHT_PIN, DHT_TYPE);
SoftwareSerial smsModule(SMS_TX, SMS_RX);

// Poultry Standard Ranges
const float TEMP_MIN = 20.0;      // Minimum temperature in Celsius
const float TEMP_MAX = 30.0;      // Maximum temperature in Celsius
const float HUMIDITY_MIN = 50.0;  // Minimum humidity in %
const float HUMIDITY_MAX = 70.0;  // Maximum humidity in %
const int GAS_THRESHOLD = 400;    // Gas sensor threshold (adjust based on your sensor)

// Phone numbers for SMS alerts
const String PHONE1 = "+2347063662272";
const String PHONE2 = "+2348066459317";

// Timing variables
unsigned long buzzerStartTime = 0;
unsigned long buzzerSilentTime = 0;
bool buzzerActive = false;
bool buzzerSilent = false;
bool alertSent = false;

// Sensor readings
float temperature = 0;
float humidity = 0;
int gasLevel = 0;

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);
  
  // Initialize SMS module
  smsModule.begin(9600);
  delay(1000);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Initialize DHT sensor
  dht.begin();
  
  // Initialize buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Display welcome messages
  displayWelcome();
  
  // Initialize SMS module
  initializeSMS();
}

void loop() {
  // Read sensors
  readSensors();
  
  // Display sensor readings
  displayReadings();
  
  // Check for abnormal conditions
  bool isAbnormal = checkConditions();
  
  // Handle buzzer alert
  if (isAbnormal) {
    handleBuzzerAlert();
    
    // Send SMS alert once when abnormality is detected
    if (!alertSent) {
      sendSMSAlert();
      alertSent = true;
    }
  } else {
    // Reset alert status when conditions are normal
    digitalWrite(BUZZER_PIN, LOW);
    buzzerActive = false;
    buzzerSilent = false;
    alertSent = false;
  }
  
  delay(2000);  // Update every 2 seconds
}

void displayWelcome() {
  // First welcome message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print("  VET KONNECT");
  delay(2000);
  
  // Second welcome message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    POULTRY");
  lcd.setCursor(0, 1);
  lcd.print("HEALTH MONITOR");
  delay(3000);
}

void readSensors() {
  // Read temperature and humidity
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  // Read gas sensor
  gasLevel = analogRead(GAS_SENSOR_PIN);
  
  // Check for DHT sensor errors
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    temperature = 0;
    humidity = 0;
  }
  
  // Debug output
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" C, Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, Gas: ");
  Serial.println(gasLevel);
}

void displayReadings() {
  lcd.clear();
  
  // First line: Temperature and Humidity
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C H:");
  lcd.print(humidity, 0);
  lcd.print("%");
  
  // Second line: Gas level
  lcd.setCursor(0, 1);
  lcd.print("Gas:");
  lcd.print(gasLevel);
  
  // Add status indicator
  if (checkConditions()) {
    lcd.setCursor(11, 1);
    lcd.print("ALERT");
  } else {
    lcd.setCursor(12, 1);
    lcd.print("OK");
  }
}

bool checkConditions() {
  bool tempAbnormal = (temperature < TEMP_MIN || temperature > TEMP_MAX) && temperature != 0;
  bool humidityAbnormal = (humidity < HUMIDITY_MIN || humidity > HUMIDITY_MAX) && humidity != 0;
  bool gasAbnormal = (gasLevel > GAS_THRESHOLD);
  
  return (tempAbnormal || humidityAbnormal || gasAbnormal);
}

void handleBuzzerAlert() {
  unsigned long currentTime = millis();
  
  if (!buzzerActive && !buzzerSilent) {
    // Start buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    buzzerActive = true;
    buzzerStartTime = currentTime;
  }
  
  if (buzzerActive && (currentTime - buzzerStartTime >= 5000)) {
    // Stop buzzer after 5 seconds
    digitalWrite(BUZZER_PIN, LOW);
    buzzerActive = false;
    buzzerSilent = true;
    buzzerSilentTime = currentTime;
  }
  
  if (buzzerSilent && (currentTime - buzzerSilentTime >= 2000)) {
    // Restart buzzer after 2 seconds of silence
    buzzerSilent = false;
  }
}

void initializeSMS() {
  delay(1000);
  smsModule.println("AT");
  delay(1000);
  smsModule.println("AT+CMGF=1");  // Set SMS to text mode
  delay(1000);
}

void sendSMSAlert() {
  String message = "ALERT! Poultry Health Abnormal:\n";
  
  // Add specific abnormalities to message
  if (temperature < TEMP_MIN || temperature > TEMP_MAX) {
    message += "Temp: " + String(temperature, 1) + "C (Normal: " + String(TEMP_MIN, 0) + "-" + String(TEMP_MAX, 0) + "C)\n";
  }
  if (humidity < HUMIDITY_MIN || humidity > HUMIDITY_MAX) {
    message += "Humidity: " + String(humidity, 0) + "% (Normal: " + String(HUMIDITY_MIN, 0) + "-" + String(HUMIDITY_MAX, 0) + "%)\n";
  }
  if (gasLevel > GAS_THRESHOLD) {
    message += "Gas Level: " + String(gasLevel) + " (Threshold: " + String(GAS_THRESHOLD) + ")\n";
  }
  
  // Send to second number
  sendSMS(PHONE2, message);
  delay(2000);
  
  // Send to first number
  sendSMS(PHONE1, message);
  delay(2000);
  
  // Display SMS sent confirmation
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  SMS SENT TO");
  lcd.setCursor(0, 1);
  lcd.print("  BOTH NUMBERS");
  delay(3000);
}

void sendSMS(String phoneNumber, String message) {
  smsModule.println("AT+CMGS=\"" + phoneNumber + "\"");
  delay(1000);
  smsModule.print(message);
  delay(500);
  smsModule.write(26);  // Send Ctrl+Z to send the SMS
  delay(1000);
  
  Serial.println("SMS sent to: " + phoneNumber);
}
