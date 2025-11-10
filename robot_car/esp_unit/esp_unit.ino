#include <SoftwareSerial.h>
#include <GeoLinker.h>

SoftwareSerial gpsSerial(D1, D2);

// WiFi Network Credentials
const char* ssid = "Redmi 12";          // Your WiFi network name (SSID)
const char* password = "Congressman";  // Your WiFi network password

// API Authentication
const char* apiKey = "MJx8PvakTbix";      // Your unique GeoLinker API key
const char* deviceID = "ESP8266_CAR_Tracker";  // Unique identifier for this device
// Data Transmission Settings
const uint16_t updateInterval = 30;       // How often to send data (seconds)
const bool enableOfflineStorage = true;  // Store data when offline
const uint8_t offlineBufferLimit = 20;   // Maximum offline records to store
                                         // Keep minimal for MCUs with limited RAM
// Connection Management
const bool enableAutoReconnect = true;  // Automatically reconnect to WiFi
                                        // Note: Only applies to WiFi, ignored with GSM
// Timezone Configuration
const int8_t timeOffsetHours = 1;     // Timezone hours offset from UTC
const int8_t timeOffsetMinutes = 0;  // Timezone minutes offset from UTC
                                      // Example: IST = UTC+5:30
// Create GeoLinker instance
GeoLinker geo;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  delay(1000);  // Allow serial to initialize
  Serial.println("Starting GeoLinker GPS Tracker...");
  // Initialize GPS serial communication with custom pins
  gpsSerial.begin(9600);
  Serial.println("GPS Serial initialized on pins 16(RX) and 17(TX)");
  // ==================================================================
  //                   GEOLINKER LIBRARY SETUP
  // ==================================================================
  // Initialize GeoLinker with GPS serial interface
  geo.begin(gpsSerial);
  Serial.println("GeoLinker library initialized");
  // Configure API authentication
  geo.setApiKey(apiKey);
  Serial.println("API key configured");
  // Set unique device identifier
  geo.setDeviceID(deviceID);
  Serial.println("Device ID set");
  // Configure data transmission interval
  geo.setUpdateInterval_seconds(updateInterval);
  Serial.print("Update interval set to ");
  Serial.print(updateInterval);
  Serial.println(" seconds");
  // Set debug verbosity level
  // Options: DEBUG_NONE, DEBUG_BASIC, DEBUG_VERBOSE
  geo.setDebugLevel(DEBUG_BASIC);
  Serial.println("Debug level set to BASIC");
  // Enable offline data storage capability
  geo.enableOfflineStorage(enableOfflineStorage);
  if (enableOfflineStorage) {
    Serial.println("Offline storage enabled");
  }
  // Enable automatic WiFi reconnection
  geo.enableAutoReconnect(enableAutoReconnect);
  if (enableAutoReconnect) {
    Serial.println("Auto-reconnect enabled");
  }
  // Set maximum offline buffer size (important for memory management)
  geo.setOfflineBufferLimit(offlineBufferLimit);
  Serial.print("Offline buffer limit set to ");
  Serial.print(offlineBufferLimit);
  Serial.println(" records");
  // Configure timezone offset for accurate timestamps
  geo.setTimeOffset(timeOffsetHours, timeOffsetMinutes);
  Serial.print("Timezone offset set to UTC+");
  Serial.print(timeOffsetHours);
  Serial.print(":");
  Serial.println(timeOffsetMinutes);
  // ==================================================================
  //                    NETWORK CONNECTION SETUP
  // ==================================================================
  // Configure for WiFi mode (alternative: GEOLINKER_GSM for cellular)
  geo.setNetworkMode(GEOLINKER_WIFI);
  Serial.println("Network mode set to WiFi");
  // Set WiFi network credentials
  geo.setWiFiCredentials(ssid, password);
  Serial.println("WiFi credentials configured");
  // Attempt WiFi connection
  Serial.print("Connecting to WiFi network: ");
  Serial.println(ssid);
  if (!geo.connectToWiFi()) {
    Serial.println("ERROR: WiFi connection failed!");
    Serial.println("Device will continue with offline storage mode");
  } else {
    Serial.println("WiFi connected successfully!");
  }
  Serial.println("\n" + String("=").substring(0, 50));
  Serial.println("GeoLinker GPS Tracker setup complete!");
  Serial.println("Starting main tracking loop...");
  Serial.println(String("=").substring(0, 50) + "\n");
}

void loop() {
  geo.loop();
  delay(100);
}