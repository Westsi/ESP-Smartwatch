#include "phone_connection.h"

#include <Arduino.h>

#include <string>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID        "871a36f6-249b-4b6b-91ca-dd48f4dfd762"
#define CHARACTERISTIC_UUID "22b4ac11-86b5-4761-88d1-7e7ac380e61b"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
std::string value = "!!";
uint16_t connectionID = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("Device Connected");
        deviceConnected = true;
        connectionID = pServer->getConnId();
    }

    void onDisconnect(BLEServer* pServer) {
        Serial.println("Disconnected");
        deviceConnected = false;
    }

};

void bt_setup() {
    Serial.println("Starting BLE work!");
    BLEDevice::init("XSWatch");

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ   | 
        BLECharacteristic::PROPERTY_WRITE  | 
        BLECharacteristic::PROPERTY_NOTIFY | 
        BLECharacteristic::PROPERTY_INDICATE
    );
    pCharacteristic->addDescriptor(new BLE2902());
    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->setAppearance(ESP_BLE_APPEARANCE_GENERIC_WATCH);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void bt_loop() {
    if (deviceConnected) {
        pCharacteristic->setValue(value);
        pCharacteristic->notify();
        BLEClient* clientble = ((BLEClient*)pServer->getPeerDevices(true)[connectionID].peer_device);
        value = clientble->getPeerAddress().toString();
        Serial.print("Connected to ");
        Serial.println(value.c_str());
        delay(1000);
    }
    if (!deviceConnected && oldDeviceConnected) {
        delay(500);
        pServer->startAdvertising();
        Serial.println("Start advertising");
        oldDeviceConnected = deviceConnected;
    }
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}