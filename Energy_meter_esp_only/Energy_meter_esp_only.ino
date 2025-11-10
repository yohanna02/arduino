#include "thingProperties.h"
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include "ACS712.h"
#include <ZMPT101B.h>

#define PIR_PIN 35   // PIR sensor input
#define TRIG_PIN 19  // Ultrasonic trigger
#define ECHO_PIN 4   // Ultrasonic echo

#define ACTUAL_VOLTAGE 220.0f
#define TOLLERANCE 3.0f
#define MAX_TOLLERANCE_VOLTAGE (ACTUAL_VOLTAGE + TOLLERANCE)
#define MIN_TOLLERANCE_VOLTAGE (ACTUAL_VOLTAGE - TOLLERANCE)

#define FAST_STEP 5.0f
#define FINE_STEP 0.25f

#define VOLTAGE_SENSOR_PIN 39
#define CURRENT_SENSOR_PIN 34

#define RELAY_PIN 18
#define BUZZER_PIN 23

LiquidCrystal_I2C lcd(0x27, 20, 4);

ACS712 current_sensor(ACS712_05B, CURRENT_SENSOR_PIN);
ZMPT101B voltageSensor(VOLTAGE_SENSOR_PIN, 50.0);

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 32, 33, 25, 26 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 27, 14, 12, 13 };  //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

unsigned long lastEnergyUpdate = 0;

bool lock = false;

HardwareSerial mySerial(2);  // Use UART1

#define sim800 mySerial

String otp_pin = "";
String entered_otp_pin = "";

void setup() {
  sim800.begin(9600, SERIAL_8N1, 16, 17);
  // Ultrasonic setup
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  lcd.init();
  lcd.backlight();
  delay(1500);
  pinMode(RELAY_PIN, OUTPUT);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  // setDebugMessageLevel(2);
  // ArduinoCloud.printDebugInfo();


  lcd.print(F("Calibrating"));
  lcd.setCursor(0, 1);
  lcd.print(F("System"));

  current_sensor.calibrate();

  float sensitivity = 0.0f;
  float voltageNow;

  // while (sensitivity <= 1000.0f) {
  //   voltageSensor.setSensitivity(sensitivity);
  //   voltageNow = voltageSensor.getRmsVoltage();

  //   if (voltageNow >= MIN_TOLLERANCE_VOLTAGE && voltageNow <= MAX_TOLLERANCE_VOLTAGE) {
  //     break;
  //   }

  //   sensitivity += FAST_STEP;
  // }

  // if (sensitivity > 1000.0f) {
  //   lcd.clear();
  //   lcd.print(F("Failed to"));
  //   lcd.setCursor(0, 1);
  //   lcd.print(F("calibrate"));
  //   while (true)
  //     ;
  // }

  // float fineSensitivity = sensitivity - FAST_STEP;
  // if (fineSensitivity < 0) fineSensitivity = 0;

  // while (fineSensitivity <= sensitivity + FAST_STEP) {
  //   voltageSensor.setSensitivity(fineSensitivity);
  //   voltageNow = voltageSensor.getRmsVoltage();

  //   if (voltageNow >= MIN_TOLLERANCE_VOLTAGE && voltageNow <= MAX_TOLLERANCE_VOLTAGE) {
  //     lcd.clear();
  //     return;
  //   }

  //   fineSensitivity += FINE_STEP;
  // }

  // lcd.clear();
  // lcd.print(F("Failed to"));
  // lcd.setCursor(0, 1);
  // lcd.print(F("calibrate"));
  // while (true)
  //   ;

  lcd.clear();

  lcd.print(F("Development of"));
  lcd.setCursor(0, 1);
  lcd.print(F("power theft detection"));
  lcd.setCursor(0, 2);
  lcd.print(F("system using IoT and"));
  lcd.setCursor(0, 2);
  lcd.print(F("smart sensors with"));
  delay(3000);
  lcd.clear();
  
  lcd.print(F("prepaid functionality"));
  lcd.setCursor(0, 1);
  lcd.print(F("and mobile recharge"));
  lcd.setCursor(0, 2);
  lcd.print(F("interaction"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Karma Prince Jonah"));
  lcd.setCursor(0, 1);
  lcd.print(F("20/57182D/2"));
  lcd.setCursor(0, 2);
  lcd.print(F("Emmanuel Michael Chibiyo"));
  lcd.setCursor(0, 2);
  lcd.print(F("18/51546U/2"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Dr.Hassan Sabo Miya"));
  delay(3000);
  lcd.clear();

}

void loop() {
  ArduinoCloud.update();
  // Your code here
  
  long distance = getDistance();
  if ((distance > 4) && (distance < 15) && !lock) {
    digitalWrite(BUZZER_PIN, HIGH);
    lock = true;
    digitalWrite(RELAY_PIN, LOW);
    lcd.clear();

    uint32_t random_num = esp_random();
    int otp = random_num % 10000;
    otp_pin = String(otp);

    lcd.print(F("Temper Detected"));
    lcd.setCursor(0, 1);
    lcd.print(F("Meter Locked"));
    sendSMS("Temper Detected\nMeter Locked\nUnlock pin: " + otp_pin);
    delay(2000);
    digitalWrite(BUZZER_PIN, LOW);
  }

  char key = keypad.getKey();

  if (key == 'A' && lock) {
    lcd.clear();
    lcd.print(F("Enter unlock pin"));
    delay(1000);

    while (1) {
      char key = keypad.getKey();

      if (key >= '0' && key <= '9') {
        entered_otp_pin += key;
        lcd.setCursor(0, 1);
        lcd.print(entered_otp_pin);
        lcd.print(F("   "));
      } else if (key == '*') {
        lcd.setCursor(0, 1);
        lcd.print(F("                "));
        entered_otp_pin = "";
      } else if (key == '#') {
        lcd.clear();
        if (entered_otp_pin == otp_pin) {
          otp_pin = "";
          lock = false;
          lcd.print(F("Meter Unlocked"));
          delay(3000);
          break;
        } else {
          lcd.print(F("Invalid Pin"));
          delay(3000);
          lcd.clear();
          lcd.print(F("Enter unlock pin"));
        }
      }
    }
  } else if (key == 'D' && !lock) {
    String value = "";
    lcd.clear();
    lcd.print(F("Set Balance:"));
    while (true) {
      char key = keypad.getKey();

      if (key >= '0' && key <= '9') {
        value += key;
      }

      else if (key == '*') {
        value = "";
        lcd.clear();
        lcd.print(F("Set Balance 1:"));
      }

      else if (key == '#') {
        balance += value.toInt();
        value = "";
        break;
      }

      lcd.setCursor(0, 1);
      lcd.print(value);
    }
  }

  if (lock) return;

  float Vrms = 220;
  // float Vrms = voltageSensor.getRmsVoltage();
  float Irms = readCurrentWithCheck(current_sensor);
  float realPower = Vrms * Irms;

  unsigned long now = millis();
  float deltaTimeHours = (now - lastEnergyUpdate) / 3600000.0;
  lastEnergyUpdate = now;

  float energyThisLoop = realPower * deltaTimeHours;
  energy_consumption += energyThisLoop;

  if (balance > 0) {
    balance -= energyThisLoop;
  }

  if (balance <= 0 || lock) {
    digitalWrite(RELAY_PIN, LOW);  // Turn off power
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // Keep power on
  }

  lcd.setCursor(0, 0);
  lcd.print(F("B: "));
  lcd.print(balance);
  lcd.print(F(" Wh       "));
  lcd.setCursor(0, 1);
  lcd.print(F("En: "));
  lcd.print(energy_consumption);
  lcd.print(F(" Wh       "));
  lcd.setCursor(0, 2);
  lcd.print(F("Load: "));
  lcd.print(balance <= 0 ? "OFF  " : "ON   ");
  // lcd.setCursor(0, 3);
  // lcd.print(digitalRead(PIR_PIN));
}


/*
  Since Balance is READ_WRITE variable, onBalanceChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onBalanceChange() {
  // Add your code here to act upon Balance change
  // request(CMD_BALANCE);
}



/*
  Since Message is READ_WRITE variable, onMessageChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onMessageChange() {
  // Add your code here to act upon Message change

  if (message.startsWith("set balance:")) {
    float newBalance = message.substring(12).toFloat();

    if (newBalance == 0) {
      message = "Error updating balance";
      return;
    }

    balance += newBalance;

    message = "Balance updated";
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

// --- Function: Ultrasonic distance ---
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // Timeout 30ms
  if (duration == 0) return -1;                    // No reading
  long distance = duration * 0.034 / 2;
  return distance;
}

void sendSMS(String message) {
  sim800.println("AT+CMGF=1");  // Set SMS mode to text
  delay(500);
  sim800.println("AT+CMGS=\"+2347080948252\"");  // Replace with your phone number
  delay(500);
  sim800.print(message);
  sim800.write(26);  // Ctrl+Z to send SMS
  delay(2000);
}
