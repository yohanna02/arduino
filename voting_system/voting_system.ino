/*  ESP32 Voting System for Nigerian Political Parties
    - Displays PDP, APC, and LP party options
    - Touch to vote for a party
    - Shows vote counts when voting is complete
    - Optimized for 2.8" TFT 240x320 in landscape mode
*/

#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

TFT_eSPI tft = TFT_eSPI();

// Touchscreen pins
#define XPT2046_IRQ 36   
#define XPT2046_MOSI 32  
#define XPT2046_MISO 39  
#define XPT2046_CLK 25   
#define XPT2046_CS 33    

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// UI Colors
#define COLOR_BACKGROUND TFT_BLACK
#define COLOR_HEADER TFT_DARKGREY
#define COLOR_TEXT TFT_WHITE
#define COLOR_PDP TFT_RED
#define COLOR_APC 0x0400  // Dark green
#define COLOR_LP 0xFD20   // Orange
#define COLOR_VOTED 0x4208 // Dark grey
#define COLOR_BUTTON_GREEN 0x0400

// UI Dimensions
#define HEADER_HEIGHT 35
#define FOOTER_HEIGHT 50
#define CONTENT_HEIGHT (SCREEN_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT)

// Party vote counts
int pdpVotes = 0;
int apcVotes = 0;
int lpVotes = 0;

// Voting state
bool hasVoted = false;
bool votingComplete = false;

// Touch calibration values (adjust these for your specific display)
#define TS_MINX 300
#define TS_MAXX 3700
#define TS_MINY 200
#define TS_MAXY 3900

void setup() {
  Serial.begin(115200);

  // Initialize touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(1);

  // Initialize display
  tft.init();
  tft.setRotation(1); // Landscape mode
  tft.fillScreen(COLOR_BACKGROUND);

  drawVotingScreen();
}

void loop() {
  // Handle touch input
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    
    // Map touch coordinates to screen coordinates
    int touchX = map(p.x, TS_MINX, TS_MAXX, 0, SCREEN_WIDTH);
    int touchY = map(p.y, TS_MINY, TS_MAXY, 0, SCREEN_HEIGHT);
    
    // Constrain to screen bounds
    touchX = constrain(touchX, 0, SCREEN_WIDTH);
    touchY = constrain(touchY, 0, SCREEN_HEIGHT);
    
    Serial.print("Touch: X=");
    Serial.print(touchX);
    Serial.print(" Y=");
    Serial.println(touchY);
    
    handleTouch(touchX, touchY);
    
    delay(300); // Debounce delay
  }
}

void drawVotingScreen() {
  tft.fillScreen(COLOR_BACKGROUND);
  
  // Draw header
  drawHeader();
  
  if (votingComplete) {
    // Show results
    drawResults();
  } else {
    // Show voting options
    drawVotingOptions();
    
    // Draw complete voting button
    drawCompleteButton();
  }
}

void drawHeader() {
  // Header background
  tft.fillRect(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, COLOR_HEADER);
  
  // Title
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(2);
  
  if (votingComplete) {
    tft.setCursor(60, 10);
    tft.print("ELECTION RESULTS");
  } else {
    tft.setCursor(45, 10);
    tft.print("NIGERIAN ELECTIONS");
  }
  
  // Status indicator
  if (hasVoted && !votingComplete) {
    tft.setTextSize(1);
    tft.setTextColor(COLOR_BUTTON_GREEN);
    tft.setCursor(250, 15);
    tft.print("VOTED");
  }
}

void drawVotingOptions() {
  // Calculate dimensions for 3 party boxes
  int boxWidth = 85;
  int boxHeight = 120;
  int spacing = 15;
  int startX = (SCREEN_WIDTH - (3 * boxWidth + 2 * spacing)) / 2;
  int startY = HEADER_HEIGHT + 15;
  
  // Draw PDP
  drawPartyBox(startX, startY, boxWidth, boxHeight, "PDP", COLOR_PDP, 0);
  
  // Draw APC
  drawPartyBox(startX + boxWidth + spacing, startY, boxWidth, boxHeight, "APC", COLOR_APC, 1);
  
  // Draw LP
  drawPartyBox(startX + 2 * (boxWidth + spacing), startY, boxWidth, boxHeight, "LP", COLOR_LP, 2);
}

void drawPartyBox(int x, int y, int w, int h, const char* name, uint16_t color, int partyIndex) {
  // Draw box background
  tft.fillRoundRect(x, y, w, h, 8, color);
  tft.drawRoundRect(x, y, w, h, 8, COLOR_TEXT);
  
  // Draw party logo circle
  int circleX = x + w/2;
  int circleY = y + 35;
  int circleRadius = 25;
  tft.fillCircle(circleX, circleY, circleRadius, COLOR_TEXT);
  tft.fillCircle(circleX, circleY, circleRadius - 3, color);
  
  // Draw party abbreviation in circle
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(2);
  int textW = strlen(name) * 12;
  tft.setCursor(circleX - textW/2, circleY - 8);
  tft.print(name);
  
  // Draw "TAP TO VOTE" text
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(x + 8, y + h - 25);
  tft.print("TAP TO");
  tft.setCursor(x + 20, y + h - 13);
  tft.print("VOTE");
}

void drawCompleteButton() {
  int buttonY = SCREEN_HEIGHT - FOOTER_HEIGHT + 10;
  int buttonWidth = 150;
  int buttonHeight = 35;
  int buttonX = (SCREEN_WIDTH - buttonWidth) / 2;
  
  tft.fillRoundRect(buttonX, buttonY, buttonWidth, buttonHeight, 5, COLOR_BUTTON_GREEN);
  tft.drawRoundRect(buttonX, buttonY, buttonWidth, buttonHeight, 5, COLOR_TEXT);
  
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(buttonX + 15, buttonY + 13);
  tft.print("COMPLETE VOTING");
}

void drawResults() {
  int startY = HEADER_HEIGHT + 20;
  int rowHeight = 35;
  int barMaxWidth = 220;
  int labelX = 20;
  int barX = 90;
  
  // Calculate total votes
  int totalVotes = pdpVotes + apcVotes + lpVotes;
  
  // Title
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(2);
  tft.setCursor(95, startY);
  tft.print("FINAL TALLY");
  
  startY += 35;
  
  // PDP Results
  drawResultBar(labelX, startY, barX, barMaxWidth, "PDP", pdpVotes, totalVotes, COLOR_PDP);
  
  // APC Results
  startY += rowHeight;
  drawResultBar(labelX, startY, barX, barMaxWidth, "APC", apcVotes, totalVotes, COLOR_APC);
  
  // LP Results
  startY += rowHeight;
  drawResultBar(labelX, startY, barX, barMaxWidth, "LP", lpVotes, totalVotes, COLOR_LP);
  
  // Total votes
  startY += rowHeight + 10;
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(labelX, startY);
  tft.print("TOTAL VOTES: ");
  tft.print(totalVotes);
  
  // Reset button
  int buttonY = SCREEN_HEIGHT - 40;
  int buttonWidth = 120;
  int buttonX = (SCREEN_WIDTH - buttonWidth) / 2;
  
  tft.fillRoundRect(buttonX, buttonY, buttonWidth, 30, 5, TFT_BLUE);
  tft.drawRoundRect(buttonX, buttonY, buttonWidth, 30, 5, COLOR_TEXT);
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(buttonX + 25, buttonY + 11);
  tft.print("RESET POLL");
}

void drawResultBar(int labelX, int y, int barX, int barMaxWidth, const char* party, int votes, int total, uint16_t color) {
  // Party label
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(2);
  tft.setCursor(labelX, y);
  tft.print(party);
  
  // Calculate bar width
  int barWidth = 0;
  if (total > 0) {
    barWidth = (votes * barMaxWidth) / total;
  }
  
  // Draw bar background
  tft.drawRect(barX, y, barMaxWidth, 20, COLOR_TEXT);
  
  // Draw filled bar
  if (barWidth > 0) {
    tft.fillRect(barX + 1, y + 1, barWidth - 1, 18, color);
  }
  
  // Draw vote count
  tft.setTextSize(1);
  tft.setCursor(barX + barMaxWidth + 10, y + 6);
  tft.print(votes);
  tft.print(" (");
  if (total > 0) {
    tft.print((votes * 100) / total);
  } else {
    tft.print("0");
  }
  tft.print("%)");
}

void handleTouch(int x, int y) {
  if (!votingComplete) {
    // Check party boxes (only if not voted yet)
    if (!hasVoted && y > HEADER_HEIGHT && y < HEADER_HEIGHT + CONTENT_HEIGHT - 20) {
      int boxWidth = 85;
      int spacing = 15;
      int startX = (SCREEN_WIDTH - (3 * boxWidth + 2 * spacing)) / 2;
      int startY = HEADER_HEIGHT + 15;
      int boxHeight = 120;
      
      // Check PDP
      if (x >= startX && x <= startX + boxWidth && 
          y >= startY && y <= startY + boxHeight) {
        castVote(0); // PDP
      }
      // Check APC
      else if (x >= startX + boxWidth + spacing && 
               x <= startX + 2 * boxWidth + spacing && 
               y >= startY && y <= startY + boxHeight) {
        castVote(1); // APC
      }
      // Check LP
      else if (x >= startX + 2 * (boxWidth + spacing) && 
               x <= startX + 3 * boxWidth + 2 * spacing && 
               y >= startY && y <= startY + boxHeight) {
        castVote(2); // LP
      }
    }
    
    // Check complete voting button
    int buttonY = SCREEN_HEIGHT - FOOTER_HEIGHT + 10;
    int buttonWidth = 150;
    int buttonHeight = 35;
    int buttonX = (SCREEN_WIDTH - buttonWidth) / 2;
    
    if (x >= buttonX && x <= buttonX + buttonWidth && 
        y >= buttonY && y <= buttonY + buttonHeight) {
      completeVoting();
    }
  } else {
    // Check reset button
    int buttonY = SCREEN_HEIGHT - 40;
    int buttonWidth = 120;
    int buttonX = (SCREEN_WIDTH - buttonWidth) / 2;
    
    if (x >= buttonX && x <= buttonX + buttonWidth && 
        y >= buttonY && y <= buttonY + 30) {
      resetPoll();
    }
  }
}

void castVote(int partyIndex) {
  if (hasVoted) return;
  
  // Increment vote count
  switch(partyIndex) {
    case 0: pdpVotes++; break;
    case 1: apcVotes++; break;
    case 2: lpVotes++; break;
  }
  
  hasVoted = true;
  
  // Show confirmation
  showVoteConfirmation(partyIndex);
  
  delay(1000);
  
  // Redraw screen
  drawVotingScreen();
}

void showVoteConfirmation(int partyIndex) {
  const char* partyNames[] = {"PDP", "APC", "LP"};
  
  // Draw confirmation overlay
  int overlayX = 60;
  int overlayY = 80;
  int overlayW = 200;
  int overlayH = 80;
  
  tft.fillRoundRect(overlayX, overlayY, overlayW, overlayH, 10, COLOR_BUTTON_GREEN);
  tft.drawRoundRect(overlayX, overlayY, overlayW, overlayH, 10, COLOR_TEXT);
  
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(2);
  tft.setCursor(overlayX + 35, overlayY + 15);
  tft.print("VOTE CAST!");
  
  tft.setTextSize(1);
  tft.setCursor(overlayX + 50, overlayY + 40);
  tft.print("You voted for");
  
  tft.setTextSize(2);
  tft.setCursor(overlayX + 75, overlayY + 55);
  tft.print(partyNames[partyIndex]);
}

void completeVoting() {
  votingComplete = true;
  drawVotingScreen();
}

void resetPoll() {
  pdpVotes = 0;
  apcVotes = 0;
  lpVotes = 0;
  hasVoted = false;
  votingComplete = false;
  
  drawVotingScreen();
}