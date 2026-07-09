#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include "Arial14.h"
#include "Arial_Black_16_ISO_8859_1.h"
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

// 3 Displays Across
#define DISPLAYS_ACROSS 3
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

unsigned long prevMillis = 0;
unsigned long lastTimeUpdate = 0;

void ScanDMD() {
  dmd.scanDisplayBySPI();
}

void setup() {
  Serial.begin(9600);

  Timer1.initialize(3000);
  Timer1.attachInterrupt(ScanDMD);

  dmd.clearScreen(true);
  dmd.selectFont(Arial_14);

  if (!rtc.begin()) {
    dmd.drawString(10, 0, "RTC Error", 9, GRAPHICS_NORMAL);
    delay(5000);
  }
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  dmd.clearScreen(true);
}

void loop() {
  DateTime now = rtc.now();

  // Show date every 30 seconds
  if (millis() - prevMillis > 30000) {

    dmd.clearScreen(true);
    dmd.selectFont(Arial_Black_16_ISO_8859_1);

    char dateStr[11];
    sprintf(dateStr, "%02d-%02d-%04d",
            now.day(),
            now.month(),
            now.year());

    // Centered on 96-pixel display
    dmd.drawString(12, 2, dateStr, strlen(dateStr), GRAPHICS_NORMAL);

    delay(10000);

    dmd.clearScreen(true);
    prevMillis = millis();
  }

  // Update clock once every second
  if (millis() - lastTimeUpdate >= 1000) {

    lastTimeUpdate = millis();

    int hour = now.hour();
    bool isPM = hour >= 12;

    if (hour == 0)
      hour = 12;
    else if (hour > 12)
      hour -= 12;

    char timeStr[15];
    sprintf(timeStr,
            "%02d:%02d:%02d %s",
            hour,
            now.minute(),
            now.second(),
            isPM ? "PM" : "AM");

    dmd.clearScreen(true);

    dmd.selectFont(Arial_Black_16_ISO_8859_1);

    // Adjust X position if necessary
    dmd.drawString(7, 0, timeStr, strlen(timeStr), GRAPHICS_NORMAL);
  }
}