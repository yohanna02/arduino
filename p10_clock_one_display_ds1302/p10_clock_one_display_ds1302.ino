#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include "Arial14.h"

#include <ThreeWire.h>
#include <RtcDS1302.h>

// --- DS1302 Pin Connections ---
#define DS1302_CLK 4  // SCLK
#define DS1302_DAT 3  // IO
#define DS1302_RST 2  // CE

ThreeWire myWire(DS1302_DAT, DS1302_CLK, DS1302_RST);  // IO, SCLK, CE
RtcDS1302<ThreeWire> rtc(myWire);

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

  Timer1.initialize(3000);
  Timer1.attachInterrupt(ScanDMD);

  dmd.clearScreen(true);
  dmd.selectFont(Arial_14);

  rtc.Begin();

  // If RTC is not running, set it to compile time
  if (!rtc.IsDateTimeValid()) {
    rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
  }
    rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));

  // If RTC was stopped, start it
  if (rtc.GetIsWriteProtected()) {
    rtc.SetIsWriteProtected(false);
  }
  if (!rtc.GetIsRunning()) {
    rtc.SetIsRunning(true);
  }
}

void loop() {
  RtcDateTime now = rtc.GetDateTime();

  // Show date every 30 seconds
  if (millis() - prevMillis > 30000) {
    dmd.clearScreen(true);
    char date[6];
    sprintf(date, "%02d/%02d", now.Day(), now.Month());
    dmd.drawString(0, 2, date, 5, GRAPHICS_NORMAL);
    delay(4000);
    dmd.clearScreen(true);
    prevMillis = millis();
  }

  // Draw hour and minute once per loop
  char hourStr[3], minStr[3];
  sprintf(hourStr, "%02d", now.Hour());
  sprintf(minStr, "%02d", now.Minute());

  int hourX = 3;
  int colonX = 14;
  int minX = 17;

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
