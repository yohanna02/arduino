#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include "Arial14.h"
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

unsigned long prevMillis = 0;
unsigned long lastBlink = 0;
bool showColon = true;

void ScanDMD() {
  dmd.scanDisplayBySPI();
}

void setup() {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  Timer1.initialize(3000);
  Timer1.attachInterrupt(ScanDMD);

  dmd.clearScreen(true);
  dmd.selectFont(Arial_14);
}

void loop() {
  DateTime now = rtc.now();

  // Show date every 30 seconds
  if (millis() - prevMillis > 30000) {
    dmd.clearScreen(true);
    char date[6];
    sprintf(date, "%02d/%02d", now.day(), now.month());
    dmd.drawString(0, 2, date, 5, GRAPHICS_NORMAL);
    delay(4000);
    dmd.clearScreen(true);
    prevMillis = millis();
  }

  // Draw hour and minute once per loop
  char hourStr[3], minStr[3];
  sprintf(hourStr, "%02d", now.hour());
  sprintf(minStr, "%02d", now.minute());

  // Positioning: adjust based on your display size/font
  int hourX = 3;
  int colonX = 14;  // shift based on font width
  int minX = 17;

  // Redraw time
  dmd.drawString(hourX, 2, hourStr, 2, GRAPHICS_NORMAL);
  dmd.drawString(minX, 2, minStr, 2, GRAPHICS_NORMAL);

  // Blink colon every second
  if (millis() - lastBlink >= 1000) {
    lastBlink = millis();
    showColon = !showColon;

    if (showColon) {
      dmd.drawChar(colonX, 1, ':', GRAPHICS_NORMAL);
    } else {
      dmd.drawChar(colonX, 1, ' ', GRAPHICS_NORMAL);
    }
  }
}
