#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include <Arial_Black_16_ISO_8859_1.h>

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

String message = "Welcome to Electrical Electronics Engineering Department";

/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void ScanDMD() {
  dmd.scanDisplayBySPI();
}

void setup() {
  // put your setup code here, to run once:
  Timer1.initialize(3000);          //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
  Timer1.attachInterrupt(ScanDMD);  //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()

  //clear/init the DMD pixels held in RAM
  dmd.clearScreen(true);
  dmd.selectFont(Arial_Black_16_ISO_8859_1);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    message = Serial.readString();
    dmd.clearScreen(true);
  }

  const char *MSG = message.c_str();
  dmd.drawMarquee(MSG, strlen(MSG), (32 * DISPLAYS_ACROSS) - 1, 0);
  long start = millis();
  long timer = start;
  while (1) {
    if ((timer + 30) < millis()) {
      dmd.stepMarquee(-1, 0);
      timer = millis();
    }

    if (Serial.available()) {
      break;
    }
  }
}
