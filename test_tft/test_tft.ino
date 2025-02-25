#include <TFT_eSPI.h> // Include the TFT library

TFT_eSPI tft = TFT_eSPI(); // Create TFT object

// Define the segments of the text
const char* segments[] = {
    "Development of Functional Electrical",
    "Stimulation Device for movement",
    "restoration with respect to spinal",
    "cord injury."
};

const int numSegments = sizeof(segments) / sizeof(segments[0]);
int currentSegment = 0;

void setup() {
    tft.init();
    tft.setRotation(1); // Set rotation if needed
    tft.fillScreen(TFT_BLACK); // Clear the screen

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2); // Set a larger text size

    tft.drawCentreString("WELCOME TO FES", 160, 100, 2); // Centered
    delay(30000);
}

void loop() {
    // Clear the previous text
    tft.fillRect(0, 0, 320, 240, TFT_BLACK);

    // Draw the current segment of text
    tft.drawCentreString(segments[currentSegment], 160, 100, 2); // Centered

    // Move to the next segment
    currentSegment++;
    if (currentSegment >= numSegments) {
        currentSegment = 0; // Reset to the first segment
    }

    delay(2000); // Display each segment for 2 seconds
}
