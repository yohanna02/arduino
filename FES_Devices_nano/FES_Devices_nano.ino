#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

// Pin definitions
#define MENU_BTN A0
#define SELECT_BTN A1
#define START_BTN A2
#define IN1 5
#define IN2 6

// Display pins
#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_CLK 13

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

// Serial communication macros
#define BTSerial Serial

// Custom button class (since we're not using TFT_eWidget anymore)
class Button {
public:
  int16_t x, y, w, h;
  char* label;
  bool selected;

  Button()
    : x(0), y(0), w(0), h(0), label(nullptr), selected(false) {}

  void init(int16_t _x, int16_t _y, int16_t _w, int16_t _h, char* _label) {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    label = _label;
  }

  void draw(Adafruit_ILI9341& display) {
    uint16_t color = selected ? ILI9341_RED : ILI9341_BLACK;
    display.drawRect(x, y, w, h, color);
    display.setTextColor(color);
    display.setTextSize(2);

    int16_t textX = x + (w - strlen(label) * 12) / 2;
    int16_t textY = y + (h - 16) / 2;
    display.setCursor(textX, textY);
    display.print(label);
  }
} btnBluetooth, btnManual;

// Screen states
enum Screen { SELECT_SCREEN,
              SETTINGS_SCREEN,
              RUNNING_SCREEN };

// Settings parameters
struct Settings {
  int frequency = 5;     // Hz (0-60Hz)
  int pulseWidth = 200;  // µs (0-360µs)
  int amplitude = 0;     // mA (0-167mA)
} settings;

int period = 0;
int lastPulseWidth = 0;

Screen currentScreen = SELECT_SCREEN;
int currentSelection = 0;   // Add this line: 0 for Bluetooth, 1 for Manual
int settingsSelection = 0;  // 0: frequency, 1: pulse width, 2: intensity

// Button state variables
bool lastMenuState = HIGH;
bool lastSelectState = HIGH;
bool lastStartState = HIGH;

// Add after other global variables
unsigned long startTime = 0;
unsigned long elapsedTime = 0;
bool isRunning = false;

// Function prototypes
void drawSelectUI();
void drawSettingsUI();
void drawRunningUI();
void handleSelectScreen(bool menuState, bool selectState);
void handleSettingsScreen(bool menuState, bool selectState, bool startState);
void handleRunningScreen(bool startState);

// Add this new function before loop()
String formatTime(unsigned long timeInSeconds) {
  int hours = timeInSeconds / 3600;
  int minutes = (timeInSeconds % 3600) / 60;
  int seconds = timeInSeconds % 60;

  char timeStr[9];
  sprintf(timeStr, "%02d:%02d:%02d", hours, minutes, seconds);
  return String(timeStr);
}

void setup() {
  BTSerial.begin(9600);

  // Initialize display
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_WHITE);

  // Initialize buttons
  btnBluetooth.init(60, 100, 120, 40, "Bluetooth");
  btnManual.init(60, 160, 120, 40, "Manual");

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // Initialize button pins
  pinMode(MENU_BTN, INPUT_PULLUP);
  pinMode(SELECT_BTN, INPUT_PULLUP);
  pinMode(START_BTN, INPUT_PULLUP);

  drawSelectUI();
}

void loop() {
  bool menuState = digitalRead(MENU_BTN);
  bool selectState = digitalRead(SELECT_BTN);
  bool startState = digitalRead(START_BTN);

  switch (currentScreen) {
    case SELECT_SCREEN:
      handleSelectScreen(menuState, selectState);
      break;
    case SETTINGS_SCREEN:
      handleSettingsScreen(menuState, selectState, startState);
      break;
    case RUNNING_SCREEN:
      handleRunningScreen(startState);
      break;
  }

  lastMenuState = menuState;
  lastSelectState = selectState;
  lastStartState = startState;
}

void handleSelectScreen(bool menuState, bool selectState) {
  if (menuState == LOW && lastMenuState == HIGH) {
    currentSelection = !currentSelection;
    btnBluetooth.selected = (currentSelection == 0);
    btnManual.selected = (currentSelection == 1);
    btnBluetooth.draw(tft);
    btnManual.draw(tft);
    delay(200);
  }

  if (selectState == LOW && lastSelectState == HIGH) {
    if (currentSelection == 0) {
      // No need to initialize HC-05 here as it's already done in setup()
    }
    currentScreen = SETTINGS_SCREEN;
    drawSettingsUI();
    delay(200);
  }
}

void handleSettingsScreen(bool menuState, bool selectState, bool startState) {
  if (menuState == LOW && lastMenuState == HIGH) {
    settingsSelection = (settingsSelection + 1) % 3;
    drawSettingsUI();
    delay(200);
  }

  if (selectState == LOW && lastSelectState == HIGH) {
    switch (settingsSelection) {
      case 0:  // Frequency
        settings.frequency =
          (settings.frequency + 5) % 65;                      // 0-60Hz in steps of 5
        if (settings.frequency == 0) settings.frequency = 5;  // Minimum 5Hz

        period = 1000000 / settings.frequency;
        lastPulseWidth = period - 2 * settings.pulseWidth;
        break;
      case 1:  // Pulse Width
        settings.pulseWidth =
          (settings.pulseWidth + 20) % 380;  // 0-360µs in steps of 20
        break;
      case 2:  // Amplitude
        settings.amplitude =
          (settings.amplitude + 5) % 171;  // 0-167mA in steps of 5
        break;
    }
    drawSettingsUI();
    delay(200);
  }

  if (startState == LOW && lastStartState == HIGH) {
    currentScreen = RUNNING_SCREEN;
    startTime = millis();
    elapsedTime = 0;
    isRunning = true;
    drawRunningUI();
    delay(200);
  }
}

void handleRunningScreen(bool startState) {
  if (startState == LOW && lastStartState == HIGH) {
    isRunning = false;
    currentScreen = SETTINGS_SCREEN;
    drawSettingsUI();
    delay(200);
  }

  if (isRunning) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    delayMicroseconds(settings.pulseWidth);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    delayMicroseconds(lastPulseWidth);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    delayMicroseconds(lastPulseWidth);

    elapsedTime = (millis() - startTime) / 1000;
    // Update timer display every second
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 1000) {
      drawRunningUI();
      lastUpdate = millis();
    }
  }
}

void drawSettingsUI() {
  tft.fillScreen(ILI9341_WHITE);
  tft.drawRect(10, 10, 220, 300, ILI9341_BLACK);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 30);
  tft.print("Settings");

  // Draw parameters
  tft.setTextColor(settingsSelection == 0 ? ILI9341_RED : ILI9341_BLACK);
  tft.setCursor(50, 100);
  tft.print("Freq: " + String(settings.frequency) + "Hz");

  tft.setTextColor(settingsSelection == 1 ? ILI9341_RED : ILI9341_BLACK);
  tft.setCursor(50, 140);
  tft.print("Width: " + String(settings.pulseWidth) + "us");

  tft.setTextColor(settingsSelection == 2 ? ILI9341_RED : ILI9341_BLACK);
  tft.setCursor(50, 180);
  tft.print("Amp: " + String(settings.amplitude) + "mA");

  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(50, 240);
  tft.print("Press START");
  tft.setCursor(50, 260);
  tft.print("to begin");
}

void drawRunningUI() {
  tft.fillScreen(ILI9341_WHITE);
  tft.drawRect(10, 10, 220, 300, ILI9341_BLACK);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 30);
  tft.print("Running");

  tft.setCursor(50, 100);
  tft.print(String(settings.frequency) + "Hz");
  tft.setCursor(50, 140);
  tft.print(String(settings.pulseWidth) + "us");
  tft.setCursor(50, 180);
  tft.print(String(settings.amplitude) + "mA");

  // Display timer
  tft.setCursor(50, 210);
  tft.print(formatTime(elapsedTime));

  tft.setCursor(50, 240);
  tft.print("Press START");
  tft.setCursor(50, 260);
  tft.print("to stop");
}

void drawSelectUI() {
  tft.fillScreen(ILI9341_WHITE);
  tft.drawRect(10, 10, 220, 300, ILI9341_BLACK);

  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 30);
  tft.print("Select Control");

  btnBluetooth.draw(tft);
  btnManual.draw(tft);
}
