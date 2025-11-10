#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include "SystemFont5x7.h"
#include "Arial14.h"

// P10 Display setup
#define DISPLAYS_ACROSS 2  // two boards across (64px wide)
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

// Variables for countdown
long countdownTime = 0;  // total time in seconds
long lastUpdate = 0;     // last time display was updated
bool running = false;    // is countdown running?
bool waiting = true;     // waiting for new timer

// Refresh function
void ScanDMD() {
  dmd.scanDisplayBySPI();
}

// Scroll text once
void showMarquee(const char *text, int length) {
  dmd.clearScreen(true);
  dmd.selectFont(Arial_14);
  dmd.drawMarquee(text, length, (32 * DISPLAYS_ACROSS) - 1, 1);

  long timer = millis();
  boolean ret = false;
  while (!ret) {
    if ((timer + 30) < millis()) {
      ret = dmd.stepMarquee(-1, 0);
      timer = millis();
    }
  }
}

// Show static text centered
void showStaticText(const char *text) {
  dmd.clearScreen(true);
  dmd.selectFont(SystemFont5x7);
  dmd.drawString(5, 5, text, strlen(text), GRAPHICS_NORMAL);
}

// Show countdown number
void showCountdownNumber(long t) {
  dmd.clearScreen(true);

  int hours = t / 3600;
  int minutes = (t % 3600) / 60;
  int sec = t % 60;

  char buffer[12];
  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, sec);

  dmd.selectFont(Arial_14);
  dmd.drawString(9, 2, buffer, strlen(buffer), GRAPHICS_NORMAL);  // fits better
}

void setup() {
  // Start hardware Serial (Bluetooth connected here)
  Serial.begin(9600);

  // Init display
  Timer1.initialize(5000);
  Timer1.attachInterrupt(ScanDMD);
  dmd.clearScreen(true);

  // Startup message
  showMarquee("Reformed Levites", 17);
  delay(2000);

  // Idle message
  showStaticText("SET TIMER");
}

void loop() {
  // Check if new time was sent via Bluetooth
  if (Serial.available()) {
    String value = Serial.readString();
    if (value == "start") {
      if (countdownTime > 0) {
        lastUpdate = millis();
        running = true;
      }
      return;
    }

    if (value == "stop") {
      running = false;
      return;
    }

    long newTime = value.toInt();
    if (newTime > 0) {
      countdownTime = newTime;
      running = false;
      waiting = false;
      lastUpdate = millis();
      showCountdownNumber(countdownTime);  // show immediately
    }
  }

  // If countdown is running, update every second
  if (running && (millis() - lastUpdate >= 1000)) {
    countdownTime--;
    lastUpdate = millis();

    if (countdownTime >= 0) {
      showCountdownNumber(countdownTime);
    } else {
      // Finished
      running = false;

      // Show TIME UP
      showMarquee("TIME UP", 7);
      delay(2000);

      // Then show SET TIMER and keep it until a new time is sent
      showStaticText("SET TIMER");
      waiting = true;
    }
  }
}
