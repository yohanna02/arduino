#include "HX711.h"

#define DT 2
#define SCK 3

HX711 scale;

float calibration_factor = -18050;  // initial guess (will change)

void setup() {
  Serial.begin(9600);
  scale.begin(DT, SCK);

  Serial.println("Remove all weight from scale");
  delay(3000);

  scale.set_scale();
  scale.tare();  // reset the scale to 0

  Serial.println("Scale is ready");
}

void loop() {
  scale.set_scale(calibration_factor);

  Serial.print("Weight: ");
  Serial.print(scale.get_units(), 3);
  Serial.print(" kg");
  Serial.print("  |  Calibration Factor: ");
  Serial.println(calibration_factor);

  // Use serial monitor to adjust calibration
  if (Serial.available()) {
    char temp = Serial.read();
    if (temp == '+') calibration_factor += 10;
    if (temp == '-') calibration_factor -= 10;
  }

  delay(500);
}
