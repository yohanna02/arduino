#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEClient.h>

#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "abcdef01-1234-5678-1234-56789abcdef0"

BLEClient* pClient;
BLERemoteCharacteristic* pRemoteCharacteristic;
bool connected = false;
BLEAdvertisedDevice* myDevice;

// Callback to find the server
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
      Serial.println("✅ Found BLE Server!");
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      BLEDevice::getScan()->stop();
    }
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("");

  // Scan for BLE Server
  
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

  if (myDevice) {
    pClient = BLEDevice::createClient();
    pClient->connect(myDevice);

    BLERemoteService* pRemoteService = pClient->getService(BLEUUID(SERVICE_UUID));
    if (pRemoteService) {
      pRemoteCharacteristic = pRemoteService->getCharacteristic(BLEUUID(CHARACTERISTIC_UUID));
      if (pRemoteCharacteristic) {
        Serial.println("🔓 Connected to BLE Server (No Encryption)");
        connected = true;
      }
    }
  }
}

void loop() {
  if (connected && pRemoteCharacteristic) {
    std::string value = pRemoteCharacteristic->readValue();
    Serial.print("🔓 Data Received: ");
    Serial.println(value.c_str());
    delay(1000);
  }
}
