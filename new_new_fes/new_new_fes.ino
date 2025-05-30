#include <TimerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BLUETOOTH_SERIAL Serial

const int pwmPin = 9;          // PWM output pin (supports Timer1)
unsigned long period = 50000;  // Default period for 20 Hz
int pulseWidth = 510;

int frequency = 20;  // Hz

int amplitude = 50;

#define BTN_PULSE_WIDTH 3
#define BTN_FREQUENCY 4
#define BTN_MENU 5

bool running = false;

const int debounceDelay = 200;  // Debounce time in milliseconds
unsigned long lastDebounceTimeMenu = 0;

LiquidCrystal_I2C lcd(0x27, 16, 4);  // Adjust the I2C address if necessary

enum class OP_MODE {
  NONE,
  BLUETOOTH,
  MANUAL
};

OP_MODE sys_mode = OP_MODE::NONE;

enum class SELECT_STATE_T {
  FREQ,
  PULSE,
  AMPLITUDE
};

SELECT_STATE_T select_param_state = SELECT_STATE_T::FREQ;

unsigned long startRunningMillis = 0;
unsigned long stopRunningMillis = 0;

float totalTimeSeconds = 0;

void setup() {
  BLUETOOTH_SERIAL.begin(9600);
  pinMode(BTN_PULSE_WIDTH, INPUT_PULLUP);
  pinMode(BTN_FREQUENCY, INPUT_PULLUP);
  pinMode(BTN_MENU, INPUT_PULLUP);

  Timer1.initialize(period);  // Initialize Timer1 with the default period

  lcd.init();       // Initialize the LCD
  lcd.backlight();  // Turn on the backlight

  lcd.print(F("Development/"));
  lcd.setCursor(0, 1);
  lcd.print(F("Construction of"));
  lcd.setCursor(0, 2);
  lcd.print(F("of a functional"));
  lcd.setCursor(0, 3);
  lcd.print(F("electrical"));
  delay(3000);
  lcd.clear();

  lcd.print(F("stimulation"));
  lcd.setCursor(0, 1);
  lcd.print(F("device for"));
  lcd.setCursor(0, 2);
  lcd.print(F("movement"));
  lcd.setCursor(0, 3);
  lcd.print(F("restoration"));
  delay(3000);
  lcd.clear();

  lcd.print(F("in subjects"));
  lcd.setCursor(0, 1);
  lcd.print(F("with spinal"));
  lcd.setCursor(0, 2);
  lcd.print(F("cord injuries"));
  delay(3000);
  lcd.clear();

  lcd.print(F("By"));
  lcd.setCursor(0, 1);
  lcd.print(F("Nasiru"));
  lcd.setCursor(0, 2);
  lcd.print(F("Abdulsalam"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised By"));
  lcd.setCursor(0, 1);
  lcd.print(F("Dr.A.Mohammed"));
  lcd.setCursor(0, 2);
  lcd.print(F("Dr.A.Hassan"));
  delay(3000);
  lcd.clear();
}

void loop() {
  while (sys_mode == OP_MODE::NONE) {
    lcd.setCursor(0, 0);
    lcd.print(F("Select Mode"));
    lcd.setCursor(0, 1);
    lcd.print(F("1. Bluetooth"));
    lcd.setCursor(0, 2);
    lcd.print(F("2. Manual"));

    int readBluetooth = digitalRead(BTN_FREQUENCY);
    int readingManual = digitalRead(BTN_PULSE_WIDTH);

    if (readBluetooth == LOW) {
      delay(debounceDelay);

      sys_mode = OP_MODE::BLUETOOTH;

      lcd.clear();
      lcd.print(F("Bluetooth Mode"));
      delay(3000);
      lcd.clear();
    } else if (readingManual == LOW) {
      delay(debounceDelay);

      sys_mode = OP_MODE::MANUAL;

      lcd.clear();
      lcd.print(F("Manual Mode"));
      delay(3000);
      lcd.clear();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Frequecy"));
      select_param_state = SELECT_STATE_T::FREQ;
    }
  }

  int readingMenu = digitalRead(BTN_MENU);
  if (readingMenu == LOW && (millis() - lastDebounceTimeMenu) > (debounceDelay + 100)) {
    lastDebounceTimeMenu = millis();
    if (running) {
      Timer1.disablePwm(pwmPin);
      running = false;
      lcd.clear();
      lcd.print("Stopped");
      stopRunningMillis = millis();

      totalTimeSeconds = (stopRunningMillis - startRunningMillis) * 0.001;

      lcd.setCursor(0, 1);
      lcd.print(F("Time: "));
      lcd.print(totalTimeSeconds);
      lcd.print(F(" s"));

      delay(2000);
    }
  }

  while (!running) {
    if (BLUETOOTH_SERIAL.available()) {
      int freq = BLUETOOTH_SERIAL.readStringUntil(':').toInt();
      int amplitude = BLUETOOTH_SERIAL.readStringUntil(':').toInt();
      int pulse = BLUETOOTH_SERIAL.readString().toInt();

      if (sys_mode == OP_MODE::MANUAL) {
        continue;
      }

      period = (1.0 / constrain(freq, 20, 60)) * 1000000;
      pulseWidth = constrain(pulse, 0, 360);

      int realPulse = map(pulseWidth, 0, 360, 0, 1023);

      lcd.clear();

      Timer1.setPeriod(period);
      lcd.setCursor(0, 0);
      lcd.print(F("Freq: "));
      lcd.print(freq);
      lcd.print(F(" Hz"));

      Timer1.setPwmDuty(pwmPin, realPulse);  // Update PWM duty cycle
      lcd.setCursor(0, 1);
      lcd.print("Pulse: ");
      lcd.print(pulseWidth);
      lcd.print(F(" us"));

      lcd.setCursor(0, 2);
      lcd.print("Ampli: ");
      lcd.print(amplitude);
      lcd.print(F(" mA"));
    }

    int readingFrequency = digitalRead(BTN_FREQUENCY);
    if (readingFrequency == LOW) {
      delay(debounceDelay);

      if (sys_mode == OP_MODE::BLUETOOTH) {
        unsigned long startPressMillis = millis();
        bool longPress = false;
        while (digitalRead(BTN_FREQUENCY) == LOW) {
          if (millis() - startPressMillis > 2000) {
            sys_mode = OP_MODE::NONE;
            lcd.clear();
            longPress = true;
            break;
          }
        }

        if (longPress) {
          lcd.clear();
          lcd.print(F("Data sent"));
          String data = String(frequency) + ":" + String(amplitude) + ":" + String(pulseWidth);
          BLUETOOTH_SERIAL.print(data);
          delay(2000);
          lcd.clear();
          continue;
        }
      }

      if (select_param_state == SELECT_STATE_T::FREQ) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Pulse"));
        select_param_state = SELECT_STATE_T::PULSE;
      } else if (select_param_state == SELECT_STATE_T::PULSE) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Amplitude"));
        select_param_state = SELECT_STATE_T::AMPLITUDE;
      } else if (select_param_state == SELECT_STATE_T::AMPLITUDE) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Frequecy"));
        select_param_state = SELECT_STATE_T::FREQ;
      }
    }

    int readingPulseWidth = digitalRead(BTN_PULSE_WIDTH);
    if (readingPulseWidth == LOW) {
      delay(debounceDelay);

      if (select_param_state == SELECT_STATE_T::FREQ) {
        frequency += 10;
        if (frequency > 60) {
          frequency = 10;
        }
        period = (1.0 / frequency) * 1000000;
        Timer1.setPeriod(period);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Frequecy"));
        lcd.print(frequency);
        lcd.print(F(" Hz"));
      } else if (select_param_state == SELECT_STATE_T::PULSE) {
        pulseWidth += 10;

        if (pulseWidth > 360) {
          pulseWidth = 0;
        }

        Timer1.setPwmDuty(pwmPin, map(pulseWidth, 0, 360, 0, 1023));  // Update PWM duty cycle
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Pulse"));
        lcd.print(pulseWidth);
        lcd.print(F(" us"));
      } else if (select_param_state == SELECT_STATE_T::AMPLITUDE) {
        amplitude += 5;

        if (amplitude > 165) {
          amplitude = 0;
        }

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Amplitude"));
        lcd.print(amplitude);
        lcd.print(F(" mA"));
      }
    }

    if (digitalRead(BTN_MENU) == LOW) {
      delay(debounceDelay);

      unsigned long startPressMillis = millis();
      bool longPress = false;
      while (digitalRead(BTN_MENU) == LOW) {
        if (millis() - startPressMillis > 2000) {
          sys_mode = OP_MODE::NONE;
          lcd.clear();
          longPress = true;
          break;
        }
      }

      if (longPress) {
        break;
      }

      Timer1.pwm(pwmPin, map(pulseWidth, 0, 360, 0, 1023));
      running = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Running...      ");
      startRunningMillis = millis();
    }
  }
}
