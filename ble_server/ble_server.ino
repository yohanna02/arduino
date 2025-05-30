#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "abcdef01-1234-5678-1234-56789abcdef0"

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

void setup() {
  Serial.begin(115200);
  BLEDevice::init("ESP32_Unencrypted");

  pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  pServer->getAdvertising()->start();
  Serial.println("🔓 BLE Server Running (No Encryption)");
}

void loop() {
  pCharacteristic->setValue("Hello BLE");
  pCharacteristic->notify();
  Serial.println("Sent: Hello BLE");
  delay(2000);
}
