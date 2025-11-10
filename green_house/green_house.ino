#include <Wire.h>
#include <Adafruit_BMP085.h>     // Library for BMP180
#include <LiquidCrystal_I2C.h>   // Library for I2C LCD

// Pump pins
int pump_one   = A8;
int pump_two   = A9;
int pump_three = A10;

// LCD setup (I2C address 0x27, 20x4)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// BMP180
Adafruit_BMP085 bmp;

// Timing variables
unsigned long previousMillis = 0;
const unsigned long intervalOff = 30UL * 60UL * 1000UL; // 30 minutes
const unsigned long intervalOn  = 10UL * 60UL * 1000UL; // 10 minutes

bool pumpsOn = false;

// Animation variables
unsigned long lastAnimationUpdate = 0;
const unsigned long animationInterval = 500; // Animation speed
int animationFrame = 0;
int pumpAnimationFrame = 0;

// Icons  and animations 
byte thermometer[8][8] = {
  {B00100, B01010, B01010, B01010, B01010, B10001, B10001, B01110}, // Frame 1
  {B00100, B01010, B01010, B01010, B10001, B10001, B10001, B01110}, // Frame 2
  {B00100, B01010, B01010, B10001, B10001, B10001, B10001, B01110}, // Frame 3
};

byte pressure[8][8] = {
  {B00100, B01010, B01010, B00100, B00000, B01110, B10001, B01110}, // Frame 1
  {B00100, B01010, B01010, B00100, B00000, B01110, B11111, B01110}, // Frame 2
  {B00100, B01010, B01010, B00100, B00000, B01110, B11111, B11111}, // Frame 3
};

byte altitude[8][8] = {
  {B00100, B00100, B01110, B01110, B10101, B10101, B01110, B00100}, // Frame 1
  {B00100, B01110, B01110, B10101, B10101, B01110, B00100, B00000}, // Frame 2
  {B01110, B01110, B10101, B10101, B01110, B00100, B00000, B00000}, // Frame 3
};

byte waterDrop[8][8] = {
  {B00100, B00100, B01010, B01010, B10001, B10001, B10001, B01110}, // Frame 1
  {B00000, B00100, B00100, B01010, B01010, B10001, B10001, B01110}, // Frame 2
  {B00000, B00000, B00100, B00100, B01010, B01010, B10001, B01110}, // Frame 3
};

byte pump[8][8] = {
  {B00000, B01110, B10001, B10101, B10101, B10001, B01110, B00000}, // Frame 1
  {B00000, B01110, B10001, B10101, B10001, B01110, B00000, B00000}, // Frame 2
  {B00000, B01110, B10001, B10001, B01110, B00000, B00000, B00000}, // Frame 3
};

void createCustomChars(int charNum, byte charData[8]) {
  lcd.createChar(charNum, charData);
}

void updateAnimation() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastAnimationUpdate >= animationInterval) {
    animationFrame = (animationFrame + 1) % 3;
    pumpAnimationFrame = (pumpAnimationFrame + 1) % 3;
    lastAnimationUpdate = currentMillis;
    
    // Update Icons s
    createCustomChars(0, thermometer[animationFrame]);
    createCustomChars(1, pressure[animationFrame]);
    createCustomChars(2, altitude[animationFrame]);
    createCustomChars(3, waterDrop[animationFrame]);
    createCustomChars(4, pump[animationFrame]);
  }
}

void displayPumpStatus() {
  lcd.setCursor(0, 0);
  lcd.print("Pump Status: ");
  
  if (pumpsOn) {
    lcd.write(4); // Pump icon
    lcd.print(" ON ");
    
    // Animated water flow indicator
    lcd.setCursor(15, 0);
    lcd.print("[");
    for (int i = 0; i < 3; i++) {
      lcd.setCursor(16 + i, 0);
      if (i == pumpAnimationFrame % 3) {
        lcd.write(3); // Water drop
      } else {
        lcd.print(" ");
      }
    }
    lcd.print("]");
  } else {
    lcd.write(4); // Pump icon
    lcd.print(" OFF");
    lcd.setCursor(15, 0);
    lcd.print("        ");
  }
}

void displayCountdown() {
  lcd.setCursor(0, 1);
  unsigned long remainingTime;
  
  if (pumpsOn) {
    remainingTime = intervalOn - (millis() - previousMillis);
    lcd.print("Time Left: ");
  } else {
    remainingTime = intervalOff - (millis() - previousMillis);
    lcd.print("Next Start: ");
  }
  
  unsigned long minutes = remainingTime / 60000;
  unsigned long seconds = (remainingTime % 60000) / 1000;
  
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
  lcd.print("   ");
}

void displaySensorData() {
  // Temperature with animated thermometer
  lcd.setCursor(0, 2);
  lcd.write(0); // Thermometer icon
  lcd.print(" ");
  lcd.print(bmp.readTemperature(), 1);
  lcd.print("C ");
  
  // Pressure with animated gauge
  lcd.setCursor(10, 2);
  lcd.write(1); // Pressure icon
  lcd.print(" ");
  lcd.print(bmp.readPressure() / 100.0, 0);
  lcd.print("hPa");
  
  // Altitude with animated mountain
  lcd.setCursor(0, 3);
  lcd.write(2); // Altitude icon
  lcd.print(" ");
  lcd.print(bmp.readAltitude(), 1);
  lcd.print("m ");
  
  // Interactive pressure trend indicator
  lcd.setCursor(15, 3);
  static float lastPressure = 0;
  float currentPressure = bmp.readPressure() / 100.0;
  
  if (currentPressure > lastPressure + 0.5) {
    lcd.print("▲"); // Rising
  } else if (currentPressure < lastPressure - 0.5) {
    lcd.print("▼"); // Falling
  } else {
    lcd.print("●"); // Stable
  }
  lastPressure = currentPressure;
}

void setup() {
  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Setup pump pins
  pinMode(pump_one, OUTPUT);
  pinMode(pump_two, OUTPUT);
  pinMode(pump_three, OUTPUT);

  // Start with pumps off
  digitalWrite(pump_one, LOW);
  digitalWrite(pump_two, LOW);
  digitalWrite(pump_three, LOW);

  // Initialize BMP180
  if (!bmp.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("BMP180 not found!");
    while (1); // Stop if sensor not found
  }

  // Create initial Icons s
  createCustomChars(0, thermometer[0]);
  createCustomChars(1, pressure[0]);
  createCustomChars(2, altitude[0]);
  createCustomChars(3, waterDrop[0]);
  createCustomChars(4, pump[0]);

  lcd.setCursor(0, 0);
  lcd.print("System Starting...");
  delay(2000);
  lcd.clear();
  
  previousMillis = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  // Update animations
  updateAnimation();

  if (!pumpsOn && (currentMillis - previousMillis >= intervalOff)) {
    // Turn pumps ON
    digitalWrite(pump_one, HIGH);
    digitalWrite(pump_two, HIGH);
    digitalWrite(pump_three, HIGH);
    pumpsOn = true;
    previousMillis = currentMillis;
  }

  else if (pumpsOn && (currentMillis - previousMillis >= intervalOn)) {
    // Turn pumps OFF
    digitalWrite(pump_one, LOW);
    digitalWrite(pump_two, LOW);
    digitalWrite(pump_three, LOW);
    pumpsOn = false;
    previousMillis = currentMillis;
  }

  // Display information
  displayPumpStatus();
  displayCountdown();
  displaySensorData();

  delay(500); // Faster update for smoother animation
}