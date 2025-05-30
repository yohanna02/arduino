#include <LiquidCrystal_I2C.h>
#include <Adafruit_MLX90614.h>
#include <Servo.h>
#include <HX710AB.h>
#include <Keypad.h>

//  adjust pins to your setup
#define DATA_PIN 6
#define CLOCK_PIN 7

HX710A HX(DATA_PIN, CLOCK_PIN);

LiquidCrystal_I2C lcd(0x27, 20, 4);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

Servo myServo;

#define SERVO_PIN 6

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();

  myServo.attach(SERVO_PIN);

  HX.begin();
  if (!mlx.begin()) {
    lcd.print(F("MLX sensor"));
    lcd.setCursor(0, 1);
    lcd.print(F("Error"));
    while (1)
      ;
  };
}

void loop() {
  // put your main code here, to run repeatedly:
}
