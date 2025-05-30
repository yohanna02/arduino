#include <TimerOne.h>

const int pwmPin = 9;          // PWM output pin (supports Timer1)
unsigned long period = 50000;  // Default period for 20 Hz
int pulseWidth = 510;

int frequency = 20; // Hz

#define BTN_PULSE_WIDTH 3
#define BTN_FREQUENCY 4
#define BTN_MENU 5

bool running = false;

const int debounceDelay = 200; // Debounce time in milliseconds
unsigned long lastDebounceTimeFrequency = 0;
unsigned long lastDebounceTimePulseWidth = 0;
unsigned long lastDebounceTimeMenu = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BTN_PULSE_WIDTH, INPUT_PULLUP);
  pinMode(BTN_FREQUENCY, INPUT_PULLUP);
  pinMode(BTN_MENU, INPUT_PULLUP);

  Timer1.initialize(period);  // Initialize Timer1 with the default period
  // Timer1.pwm(pwmPin, 210);
}

void loop() {
  int readingFrequency = digitalRead(BTN_FREQUENCY);
  if (readingFrequency == LOW && (millis() - lastDebounceTimeFrequency) > debounceDelay) {
    lastDebounceTimeFrequency = millis();
    frequency += 10;
    if (frequency > 60) {
      frequency = 10;
    }
    period = (1.0 / frequency) * 1000000;
    Timer1.setPeriod(period);
    Serial.print(F("Frequency: "));
    Serial.println(frequency);
  }

  int readingPulseWidth = digitalRead(BTN_PULSE_WIDTH);
  if (readingPulseWidth == LOW && (millis() - lastDebounceTimePulseWidth) > debounceDelay) {
    lastDebounceTimePulseWidth = millis();
    pulseWidth = (pulseWidth == 1023) ? 0 : pulseWidth + 102;
    if (pulseWidth > 1023) {
      pulseWidth = 1023;
    }
    Timer1.setPwmDuty(pwmPin, pulseWidth); // Update PWM duty cycle
    Serial.print(F("Pulse-width: "));
    Serial.println(pulseWidth);
  }

  int readingMenu = digitalRead(BTN_MENU);
  if (readingMenu == LOW && (millis() - lastDebounceTimeMenu) > debounceDelay) {
    lastDebounceTimeMenu = millis();
    if (running) {
      Timer1.disablePwm(pwmPin);
      running = false;
      Serial.println(F("Stopped"));
    } else {
      Timer1.pwm(pwmPin, pulseWidth);
      Serial.println(F("Running..."));
      running = true;
    }
  }
}
