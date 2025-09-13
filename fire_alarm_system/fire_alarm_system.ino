#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <DFRobotDFPlayerMini.h>
#include <DHT.h>

// GSM: 22, 23

// ========== Pin Definitions ==========
#define DHTPIN 34
#define DHTTYPE DHT11

#define MQ1_PIN 35
#define MQ2_PIN 32
#define FIRE_SENSOR_PIN 33

#define DFPLAYER_RX 16
#define DFPLAYER_TX 17

// ========== Global Instances ==========
LiquidCrystal_I2C lcd(0x27, 16, 4);
DHT dht(DHTPIN, DHTTYPE);
HardwareSerial dfSerial(1); // Use UART1 for DFPlayer
DFRobotDFPlayerMini dfplayer;

// ========== Audio Track Enum ==========
enum AudioTracks {
  ROOM1_GAS = 1,
  ROOM2_GAS = 2,
  ROOM3_FIRE = 3,
  ROOM3_HEAT = 4
};

// ========== Keypad Setup ==========
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {23, 22, 21, 19}; // Adjust for ESP32
byte colPins[COLS] = {18, 5, 4};       // Adjust for ESP32
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ========== Thresholds ==========
int mq1Threshold = 0;
int mq2Threshold = 0;
int fireThreshold = 0;
int tempThreshold = 0;

void setup() {
  Serial.begin(115200);
  
  // LCD + DHT Init
  Wire.begin(26, 27); // SDA = 26, SCL = 27 (adjust if needed)
  lcd.init();
  lcd.backlight();
  dht.begin();

  // DFPlayer Setup
  dfSerial.begin(9600, SERIAL_8N1, DFPLAYER_RX, DFPLAYER_TX);
  if (!dfplayer.begin(dfSerial)) {
    lcd.print("DFPlayer error!");
    while (true);
  }
  dfplayer.volume(25); // 0 - 30

  lcd.setCursor(0, 0);
  lcd.print("Fire Alarm Setup");
  delay(2000);
  lcd.clear();

  // Get thresholds from keypad
  mq1Threshold = getThreshold("MQ1 Gas:");
  mq2Threshold = getThreshold("MQ2 Gas:");
  fireThreshold = getThreshold("Flame Det:");
  tempThreshold = getThreshold("Temp (C):");

  lcd.clear();
  lcd.print("System Ready");
  delay(1500);
  lcd.clear();
}

void loop() {
  int mq1 = analogRead(MQ1_PIN);
  int mq2 = analogRead(MQ2_PIN);
  int fireVal = digitalRead(FIRE_SENSOR_PIN);
  float temp = dht.readTemperature();

  lcd.setCursor(0, 0);
  lcd.print("M1:");
  lcd.print(mq1);
  lcd.print(" M2:");
  lcd.print(mq2);

  lcd.setCursor(0, 1);
  lcd.print("F:");
  lcd.print(fireVal == LOW ? "YES" : "NO ");
  lcd.print(" T:");
  lcd.print(temp, 1);

  if (mq1 > mq1Threshold) {
    playAlert(ROOM1_GAS);
  }
  if (mq2 > mq2Threshold) {
    playAlert(ROOM2_GAS);
  }
  if (fireVal == LOW && fireThreshold == 1) {
    playAlert(ROOM3_FIRE);
  }
  if (temp > tempThreshold) {
    playAlert(ROOM3_HEAT);
  }

  delay(1000);
}

void playAlert(AudioTracks track) {
  dfplayer.play((int)track);
  delay(5000); // Avoid spamming
}

// ========== Keypad Threshold Input ==========
int getThreshold(String label) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(label);
  lcd.setCursor(0, 1);
  lcd.print("Enter value:");

  String input = "";
  while (true) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      if (key == '#') {
        int value = input.toInt();
        lcd.clear();
        lcd.print("Saved: ");
        lcd.print(value);
        delay(1000);
        return value;
      } else if (key == '*') {
        input = "";
        lcd.setCursor(0, 2);
        lcd.print("Cleared       ");
      } else if (isDigit(key)) {
        input += key;
        lcd.setCursor(0, 2);
        lcd.print(input + "    ");
      }
    }
  }
}
