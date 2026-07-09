/*
  Simple ESP32 Fingerprint Attendance System

  Hardware:
  - ESP32 DevKit V1
  - AS608 / R307 fingerprint sensor
  - 16x2 I2C LCD
  - 4x4 keypad
  - Optional buzzer

  Wiring used in this sketch:
  - LCD SDA -> GPIO 21, LCD SCL -> GPIO 22
  - Fingerprint TX -> GPIO 16, Fingerprint RX -> GPIO 17
  - Keypad rows -> GPIO 32, 33, 25, 26
  - Keypad cols -> GPIO 27, 14, 13, 23
  - Buzzer + -> GPIO 4, Buzzer - -> GND
*/

#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>

// Change these three values for your network and deployed Google Apps Script URL.
const char* WIFI_SSID = "@OIC";
const char* WIFI_PASSWORD = "oichub@@1960";
const char* GOOGLE_SCRIPT_URL = "https://script.google.com/macros/s/AKfycbxNrWIFMhR_aGWTYG1snr1BDD9YXpu-mpvabwC4F8R_HysbUs4-PTcjGsj-pvWKHCkR/exec";

const int FINGERPRINT_RX_PIN = 16;
const int FINGERPRINT_TX_PIN = 17;
const int BUZZER_PIN = 4;

LiquidCrystal_I2C lcd(0x27, 16, 2);

HardwareSerial fingerSerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 32, 33, 25, 26 };
byte colPins[COLS] = { 27, 14, 13, 23 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
WiFiClientSecure secureClient;

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Attendance");
  lcd.setCursor(0, 1);
  lcd.print("using fingerprint");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("By Computer");
  lcd.setCursor(0, 1);
  lcd.print("Engineering");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("students");
  delay(2000);

  showMessage("Connecting", "WiFi...");
  connectWiFi();

  fingerSerial.begin(57600, SERIAL_8N1, FINGERPRINT_RX_PIN, FINGERPRINT_TX_PIN);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    showMessage("Fingerprint", "Ready");
  } else {
    showMessage("Fingerprint", "Not Found");
    delay(2000);
  }

  showMessage("WiFi Connected", "");
  delay(1000);
  showHomeScreen();
}

void loop() {
  reconnectWiFiIfNeeded();

  char key = keypad.getKey();

  if (key == '1') {
    attendanceMode();
    showHomeScreen();
  } else if (key == '4') {
    enrollmentMode();
    showHomeScreen();
  } else if (key == '3') {
    deleteMode();
    showHomeScreen();
  }
}

void showMessage(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1.substring(0, 16));
  lcd.setCursor(0, 1);
  lcd.print(line2.substring(0, 16));
}

void showHomeScreen() {
  showMessage("Ready", "1:Att 4:Enr 3:Del");
}

void beepSuccess() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(120);
  digitalWrite(BUZZER_PIN, LOW);
}

void beepError() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectWiFiIfNeeded() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  showMessage("Connecting", "WiFi...");
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  showMessage("WiFi Connected", "");
  delay(800);
}

void attendanceMode() {
  showMessage("Place Finger", "");

  int fingerprintID = getFingerprintID();

  if (fingerprintID > 0) {
    showMessage("Fingerprint", "Found");
    beepSuccess();
    delay(800);

    showMessage("Uploading...", "");
    uploadAttendance(fingerprintID);

    showMessage("Attendance", "Uploaded");
    beepSuccess();
  } else {
    showMessage("Unknown Finger", "");
    beepError();
  }

  delay(2000);
}

int getFingerprintID() {
  uint8_t result;

  // Wait until a finger is placed on the sensor.
  do {
    result = finger.getImage();
    delay(50);
  } while (result == FINGERPRINT_NOFINGER);

  if (result != FINGERPRINT_OK) {
    return -1;
  }

  result = finger.image2Tz();
  if (result != FINGERPRINT_OK) {
    return -1;
  }

  result = finger.fingerSearch();
  if (result != FINGERPRINT_OK) {
    return -1;
  }

  return finger.fingerID;
}

bool uploadAttendance(int fingerprintID) {
  reconnectWiFiIfNeeded();

  // Google Apps Script Web App URLs use HTTPS.
  // setInsecure keeps the sketch simple by skipping certificate setup.
  secureClient.setInsecure();

  HTTPClient http;
  http.begin(secureClient, GOOGLE_SCRIPT_URL);
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<64> doc;
  doc["fingerprint_id"] = fingerprintID;

  String requestBody;
  serializeJson(doc, requestBody);

  int httpCode = http.POST(requestBody);
  String response = http.getString();

  Serial.print("HTTP code: ");
  Serial.println(httpCode);
  Serial.print("Response: ");
  Serial.println(response);

  http.end();

  if (httpCode < 200 || httpCode >= 300) {
    return false;
  }

  StaticJsonDocument<128> responseDoc;
  DeserializationError error = deserializeJson(responseDoc, response);

  if (error) {
    Serial.print("JSON parse failed: ");
    Serial.println(error.c_str());
    return false;
  }

  const char* status = responseDoc["status"];
  const char* message = responseDoc["message"];

  Serial.print("Script status: ");
  Serial.println(status);
  Serial.print("Script message: ");
  Serial.println(message);

  return String(status) == "success";
}

void enrollmentMode() {
  int id = readIDFromKeypad("Enter ID:");

  if (id <= 0) {
    showMessage("Invalid ID", "");
    beepError();
    delay(1500);
    return;
  }

  showMessage("Enrolling...", "ID " + String(id));
  delay(1000);

  bool success = enrollFingerprint(id);

  if (success) {
    showMessage("Enrollment", "Complete");
    beepSuccess();
  } else {
    beepError();
  }

  delay(2000);
}

int readIDFromKeypad(String title) {
  String input = "";
  showMessage(title, "#=OK *=Clear");

  while (true) {
    char key = keypad.getKey();

    if (!key) {
      continue;
    }

    if (key >= '0' && key <= '9') {
      if (input.length() < 3) {
        input += key;
        showMessage(title, input);
      }
    } else if (key == '*') {
      input = "";
      showMessage(title, "#=OK *=Clear");
    } else if (key == '#') {
      if (input.length() == 0) {
        return -1;
      }
      return input.toInt();
    }
  }
}

bool enrollFingerprint(int id) {
  uint8_t result;

  showMessage("Place Finger", "");
  while ((result = finger.getImage()) != FINGERPRINT_OK) {
    if (result != FINGERPRINT_NOFINGER) {
      showMessage("Image Error", "");
      return false;
    }
    delay(50);
  }

  result = finger.image2Tz(1);
  if (result != FINGERPRINT_OK) {
    showEnrollError(result);
    return false;
  }

  showMessage("Remove Finger", "");
  delay(2000);

  while (finger.getImage() != FINGERPRINT_NOFINGER) {
    delay(50);
  }

  showMessage("Place Again", "");
  while ((result = finger.getImage()) != FINGERPRINT_OK) {
    if (result != FINGERPRINT_NOFINGER) {
      showMessage("Image Error", "");
      return false;
    }
    delay(50);
  }

  result = finger.image2Tz(2);
  if (result != FINGERPRINT_OK) {
    showEnrollError(result);
    return false;
  }

  result = finger.createModel();
  if (result != FINGERPRINT_OK) {
    if (result == FINGERPRINT_ENROLLMISMATCH) {
      showMessage("Fingerprints", "Did Not Match");
    } else {
      showMessage("Create Failed", "");
    }
    return false;
  }

  showMessage("Saving...", "");
  result = finger.storeModel(id);
  if (result != FINGERPRINT_OK) {
    showMessage("Save Failed", "");
    return false;
  }

  showMessage("Done", "");
  return true;
}

void showEnrollError(uint8_t errorCode) {
  if (errorCode == FINGERPRINT_IMAGEMESS) {
    showMessage("Image Messy", "Try Again");
  } else if (errorCode == FINGERPRINT_FEATUREFAIL) {
    showMessage("Feature Failed", "");
  } else if (errorCode == FINGERPRINT_INVALIDIMAGE) {
    showMessage("Invalid Image", "");
  } else {
    showMessage("Enroll Failed", "");
  }
}

void deleteMode() {
  int id = readIDFromKeypad("Delete ID:");

  if (id <= 0) {
    showMessage("Invalid ID", "");
    beepError();
    delay(1500);
    return;
  }

  uint8_t result = finger.deleteModel(id);

  if (result == FINGERPRINT_OK) {
    showMessage("Deleted", "Successfully");
    beepSuccess();
  } else {
    showMessage("Delete Failed", "");
    beepError();
  }

  delay(2000);
}
