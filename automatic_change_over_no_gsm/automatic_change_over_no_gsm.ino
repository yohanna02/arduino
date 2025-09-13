#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int nepa = A0;
const int gen = A1;
const int relay1 = 9;      // Controls NEPA load
const int relay2 = 10;      // Controls generator load
// const int gen_starter = 1; // Starts the generator
// const int gen_off = 9;    // Turns off the generator

bool gen_running = false;  // Tracks if the generator is running

void setup()
{
  pinMode(nepa, INPUT);
  pinMode(gen, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  // pinMode(gen_starter, OUTPUT);
  // pinMode(gen_off, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.print(F("Automatic power"));
  lcd.setCursor(0, 1);
  lcd.print(F("change over"));
  delay(3000);
  lcd.clear();

  lcd.print(F("switch between"));
  lcd.setCursor(0, 1);
  lcd.print(F("generator and mains"));
  delay(3000);
  lcd.clear();

  // lcd.print(F("Amodu Amoto David"));
  // lcd.setCursor(0, 1);
  // lcd.print(F("23/149725"));
  // delay(3000);
  // lcd.clear();

  // lcd.print(F("Supervised by"));
  // lcd.setCursor(0, 1);
  // lcd.print(F("Mrs. Gloria"));
  // delay(3000);
  // lcd.clear();
}

void loop()
{
  int nepa_value = digitalRead(nepa);

  if (nepa_value == HIGH) // NEPA power is available
  {
    if (gen_running)
    {
      // Turn off generator if it is running
      lcd.clear();
      lcd.print("NEPA POWER ON");
      lcd.setCursor(0, 1);
      lcd.print("Turning GEN OFF");
      // digitalWrite(gen_off, HIGH);
      // delay(2000);
      // digitalWrite(gen_off, LOW);
      gen_running = false; // Generator is now off
    }

    // Switch to NEPA power
    digitalWrite(relay2, LOW);
    delay(1000);
    digitalWrite(relay1, HIGH);
    lcd.clear();
    lcd.print("NEPA POWER ON");
  }
  else // NEPA power is not available
  {
    if (!gen_running)
    {
      // Start the generator
      lcd.clear();
      lcd.print("STARTING GEN...");
      // digitalWrite(gen_starter, HIGH);
      // delay(4000);
      // digitalWrite(gen_starter, LOW);
      gen_running = true; // Generator is now running
    }

    // Switch to generator power
    digitalWrite(relay1, LOW);
    delay(1000);
    digitalWrite(relay2, HIGH);
    lcd.clear();
    lcd.print("GEN POWER ON");
  }

  delay(1000); // Prevent rapid switching and reduce flickering
}
