#include <HX711.h>

// Pin connections
#define DT  2   // DOUT pin
#define SCK 3   // SCK pin

HX711 scale;

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 Calibration Sketch");
  Serial.println("Remove all weight from the scale");
  Serial.println("After readings begin, place a known weight on the scale");
  Serial.println("Press reset any time to restart calibration");

  scale.begin(DT, SCK);
  scale.set_scale();   // no scale factor yet
  scale.tare();        // reset the scale to 0

  Serial.println("Tare done. Start placing weights...");
}

void loop() {
  // Get raw average value (no calibration factor applied yet)
  Serial.print("Raw reading: ");
  Serial.println(scale.get_units(10));  // average of 10 readings

  delay(500);
}
