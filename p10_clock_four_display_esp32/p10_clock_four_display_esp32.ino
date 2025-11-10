#include <DMD32Plus.h>
#include "Arial14.h"

#define DISPLAYS_ACROSS 4  // total boards = 4
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

// Timer setup
// create a hardware timer  of ESP32
hw_timer_t *timer = NULL;

/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void IRAM_ATTR triggerScan()
{
  dmd.scanDisplayBySPI();
}

// Each pair of boards = 64px wide
#define PAIR_WIDTH 64
#define DISPLAY_HEIGHT 16

// Message buffers
const char *msg1 = "Hell World   ";
const char *msg2 = "Welcome to Nascomsoft Embedded   ";

// Scroll state
int x1, x2;  // x positions for pair1 and pair2
int textWidth1, textWidth2;

void setup() {
  timer = timerBegin(1000000L); // 1Mhz
  timerAttachInterrupt(timer, &triggerScan);
  timerAlarm(timer, 1000, true, 0); // alarm every 1ms

  dmd.selectFont(Arial_14);

  // Get text widths
  textWidth1 = stringWidth(msg1);
  textWidth2 = stringWidth(msg2);

  // Start off-screen to the right of each pair
  x1 = PAIR_WIDTH;        // for boards 1&2
  x2 = (PAIR_WIDTH * 2);  // for boards 3&4
}

void loop() {
  dmd.clearScreen(true);

  // ---- Pair 1 (boards 1 & 2) ----
  dmd.drawString(x1, 1, msg1, strlen(msg1), GRAPHICS_NORMAL);
  x1--;
  if (x1 < -textWidth1) {
    x1 = PAIR_WIDTH;  // reset when fully left
  }

  // ---- Pair 2 (boards 3 & 4) ----
  dmd.drawString(x2, 1, msg2, strlen(msg2), GRAPHICS_NORMAL);
  x2--;
  if (x2 < (PAIR_WIDTH * 2 - textWidth2)) {
    x2 = (PAIR_WIDTH * 2);  // reset to right edge of pair
  }

  delay(30);  // scroll speed
}

int stringWidth(const char *str) {
  int total = 0;
  while (*str) {
    total += dmd.charWidth(*str);  // add width of each character
    str++;
  }
  return total;
}
