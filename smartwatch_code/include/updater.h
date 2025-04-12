#ifndef BLE_UPDATER_H_
#define BLE_UPDATER_H_

#include <Arduino.h>
#include <string>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <esp_gatt_common_api.h>

#define UPDATER__USE_SPIFFS // comment out to use FFat instead

#define UPDATER__FORMAT_SPIFFS_IF_FAILED true
#define UPDATER__FORMAT_FFAT_IF_FAILED true

#ifdef UPDATER__USE_SPIFFS
#define UPDATER__FLASH SPIFFS
#define UPDATER__FASTMODE false    //SPIFFS write is slow
#else
#define UPDATER__FLASH FFat
#define UPDATER__FASTMODE true    //FFat is faster
#endif

#define UPDATER__NORMAL_MODE   0   // normal
#define UPDATER__UPDATE_MODE   1   // receiving firmware
#define UPDATER__OTA_MODE      2   // installing firmware

#define UPDATER_SERVICE_UUID "f0500802-0513-4aa5-82af-ec31bcd89eee"
#define UPDATER_TX_CHAR_UUID "ddbb2011-1ea6-47e3-b40b-9578c08ab5ef"
#define UPDATER_RX_CHAR_UUID "6a0e8ce6-2e06-4434-aea8-71c3aea3465e"

void updateFirmware(BLECharacteristic* pChar);
void initUpdaterBLE(BLEServer* server, BLEAdvertising *pAdvertising);
void initUpdaterFS();

void updaterHandleUpdateMode();
void updaterHandleOTAMode();

extern int UPDATER__MODE;

#endif