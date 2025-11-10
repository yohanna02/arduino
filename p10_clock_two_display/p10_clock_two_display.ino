#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include "Arial14.h"
#include "Arial_Black_16_ISO_8859_1.h"
#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 rtc;

#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

unsigned long prevMillis = 0;
unsigned long lastTimeUpdate = 0;
char lastShownTime[9] = "";  // hh:mm:ss

void ScanDMD() {
  dmd.scanDisplayBySPI();
}

// Scroll helper
void scrollText(const char *msg, int speed = 30) {
  dmd.clearScreen(true);
  dmd.selectFont(Arial_14);

  dmd.drawMarquee(msg, strlen(msg), (32 * DISPLAYS_ACROSS) - 1, 1);
  long timer = millis();
  while (true) {
    if ((timer + speed) < millis()) {
      if (dmd.stepMarquee(-1, 0)) {
        break;  // finished scrolling
      }
      timer = millis();
    }
  }
}

void setup() {
  Serial.begin(9600);


  Timer1.initialize(3000);
  Timer1.attachInterrupt(ScanDMD);

  dmd.clearScreen(true);
  dmd.selectFont(Arial_14);

  // === Scroll intro messages once ===
  if (!rtc.begin()) {
    dmd.drawString(4, 0, "RTC Error", 9, GRAPHICS_NORMAL);
    // while (1) delay(10);
    delay(5000);
    dmd.clearScreen(true);
  }
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // scrollText("WELLCOME TO COMPUTER ENGINEERING DEPARTMENT, SCHOOL OF ENGINEERING TECHNOLOGY, BINYAMINU USMAN POLYTECHNIC, HADEJIA.");

  dmd.clearScreen(true);  // clean before showing clock
}

void loop() {
  DateTime now = rtc.now();

  // scrollText("WELLCOME TO COMPUTER ENGINEERING DEPARTMENT, SCHOOL OF ENGINEERING TECHNOLOGY, BINYAMINU USMAN POLYTECHNIC, HADEJIA.");

  // char timeAndDate[] = "hh:mm - DD/MM/YYYY";
  // now.toString(timeAndDate);

  // scrollText(String(timeAndDate).c_str());

  // Show date every 30 seconds
  if (millis() - prevMillis > 30000) {
    dmd.clearScreen(true);
    dmd.selectFont(Arial_14);
    char date[] = "DD-MM-YYYY";
    now.toString(date);
    dmd.drawString(2, 2, date, 10, GRAPHICS_NORMAL);
    delay(4000);
    dmd.clearScreen(true);
    prevMillis = millis();
  }

  // Update time only once per second
  if (millis() - lastTimeUpdate >= 1000) {
    dmd.selectFont(Arial_Black_16_ISO_8859_1);
    lastTimeUpdate = millis();

    char timeStr[] = "hh:mm:ss";
    now.toString(timeStr);

    // Update only if different (prevents unnecessary redraw)
    // if (strcmp(timeStr, lastShownTime) != 0) {
    strcpy(lastShownTime, timeStr);
    dmd.clearScreen(true);
    dmd.drawString(4, 0, timeStr, 8, GRAPHICS_NORMAL);
    // }
  }
}
