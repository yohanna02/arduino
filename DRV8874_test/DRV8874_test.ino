//This code is for testing DRV8874 Brushed DC Motor Driver Board
// Code Ceated By Rajkumar Sharma www.rajkumarsharma.com

// int PWM = 6;
// int DIR = 5;
#define IN1 6
#define IN2 5
void setup() {
  // put your setup code here, to run once:
  // Serial.begin(9600);
  // pinMode(PWM, OUTPUT);
  // pinMode(DIR, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  TCCR1B = (TCCR1B & 0b11111000) | 0x01;
}

void loop() {
  //get PWM signal with 10kHz
  //Frequency and Duty cycle can be changed by changing the delay time.

  // digitalWrite(PWM, LOW);
  // delayMicroseconds(50);
  // digitalWrite(PWM, HIGH);
  // delayMicroseconds(50);
  // digitalWrite(DIR, HIGH);  // motor dir ccw)
  // delay(2000);              // wait for 2 second
  // digitalWrite(DIR, LOW);   // motor dir cw
  // delay(2000);

  // int period = 1000000 / 10;
  // int lastPulseWidth = period - 2 * 200;

  // digitalWrite(IN1, HIGH);
  // digitalWrite(IN2, LOW);
  // delayMicroseconds(200);

  // digitalWrite(IN1, LOW);
  // digitalWrite(IN2, HIGH);
  // delayMicroseconds(lastPulseWidth);

  // digitalWrite(IN1, LOW);
  // digitalWrite(IN2, LOW);
  // delayMicroseconds(lastPulseWidth);

  analogWrite(IN1, 150);
  digitalWrite(IN2, LOW);
}