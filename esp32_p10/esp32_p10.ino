#include <SPI.h>
#include <DMD32.h>
#include "fonts/SystemFont5x7.h"

#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1

DMD display(DISPLAYS_ACROSS, DISPLAYS_DOWN);

void setup() {
  display.clearScreen(true);
  display.selectFont(SystemFont5x7);

  display.drawString(0, 0, "HELLO", 5, GRAPHICS_NORMAL);
}

void loop() {
}