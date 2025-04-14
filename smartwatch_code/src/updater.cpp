#include "updater.h"
#include "ble.h"

#include "declarations.h"

#include <Arduino.h>
#include <string>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <esp_gatt_common_api.h>

#include <Update.h>
#include "FS.h"
#include "FFat.h"
#include "SPIFFS.h"

/*
Much of this code from 
https://github.com/fbiego/ESP32_BLE_OTA_Arduino.
Many thanks.
*/

BLEService* updaterService = NULL;
BLECharacteristic* updaterRXChar = NULL;
BLECharacteristic* updaterTXChar = NULL;

uint8_t updater[16384];
uint8_t updater2[16384];

static bool sendMode = false, sendSize = true;
static bool writeFile = false, request = false;
static int writeLen = 0, writeLen2 = 0;
static bool current = true;
static int parts = 0, next = 0, cur = 0, MTU = 0;
int UPDATER__MODE = UPDATER__NORMAL_MODE;
unsigned long rParts, tParts;

static void rebootEspWithReason(String reason) {
    Serial.println(reason);
    delay(1000);
    ESP.restart();
}

class MyUpdaterCharacteristicCallbacks: public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic* pChar) {
        updateFirmware(pChar);
    }

    void onNotify(BLECharacteristic *pCharacteristic) {
        uint8_t* pData;
        std::string value = pCharacteristic->getValue();
        int len = value.length();
        pData = pCharacteristic->getData();
        if (pData != NULL) {
            //        Serial.print("Notify callback for characteristic ");
            //        Serial.print(pCharacteristic->getUUID().toString().c_str());
            //        Serial.print(" of data length ");
            //        Serial.println(len);
            Serial.print("TX  ");
            for (int i = 0; i < len; i++) {
                Serial.printf("%02X ", pData[i]);
            }
            Serial.println();
        }
    }
};

void updateFirmware(BLECharacteristic* pChar) {
    uint8_t* pData;
    std::string value = pChar->getValue();
    int len = value.length();
    pData = pChar->getData();
    if (pData == NULL) {
        Serial.println("pData is NULL in updateFirmware");
    }
    if (pData[0] == 0xFB) {
        int pos = pData[1];
        for (int x = 0; x < len - 2; x++) {
            if (current) {
                updater[(pos * MTU) + x] = pData[x + 2];
            } else {
                updater2[(pos * MTU) + x] = pData[x + 2];
            } 
        }
    } else if  (pData[0] == 0xFC) {
        if (current) {
            writeLen = (pData[1] * 256) + pData[2];
        } else {
            writeLen2 = (pData[1] * 256) + pData[2];
        }
        current = !current;
        cur = (pData[3] * 256) + pData[4];
        writeFile = true;
        if (cur < parts - 1) {
            request = !UPDATER__FASTMODE;
        }
    } else if (pData[0] == 0xFD) {
        sendMode = true;
        if (UPDATER__FLASH.exists("/update.bin")) {
            UPDATER__FLASH.remove("/update.bin");
        }
    } else if (pData[0] == 0xFE) {
        rParts = 0;
        tParts = (pData[1] * 256 * 256 * 256) + (pData[2] * 256 * 256) + (pData[3] * 256) + pData[4];

        Serial.print("Available space: ");
        Serial.println(UPDATER__FLASH.totalBytes() - UPDATER__FLASH.usedBytes());
        Serial.print("File Size: ");
        Serial.println(tParts);

    } else if  (pData[0] == 0xFF) {
        parts = (pData[1] * 256) + pData[2];
        MTU = (pData[3] * 256) + pData[4];
        UPDATER__MODE = UPDATER__UPDATE_MODE;
    } else if (pData[0] == 0xEF) {
        UPDATER__FLASH.format();
        sendSize = true;
    }
}

static void writeBinary(fs::FS &fs, const char * path, uint8_t *dat, int len) {
    //Serial.printf("Write binary file %s\r\n", path);
    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("- failed to open file for writing");
        return;
    }
    file.write(dat, len);
    file.close();
    writeFile = false;
    rParts += len;
}

void sendOtaResult(String result) {
    updaterTXChar->setValue(result.c_str());
    updaterTXChar->notify();
    delay(200);
}

void performUpdate(Stream &updateSource, size_t updateSize) {
    char s1 = 0x0F;
    String result = String(s1);
    if (Update.begin(updateSize)) {
        size_t written = Update.writeStream(updateSource);
        if (written == updateSize) {
            Serial.println("Written : " + String(written) + " successfully");
        } else {
            Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
        }
        result += "Written : " + String(written) + "/" + String(updateSize) + " [" + String((written / updateSize) * 100) + "%] \n";
        if (Update.end()) {
            Serial.println("OTA done!");
            result += "OTA Done: ";
            if (Update.isFinished()) {
                Serial.println("Update successfully completed. Rebooting...");
                result += "Success!\n";
            }
            else {
                Serial.println("Update not finished? Something went wrong!");
                result += "Failed!\n";
            }
        } else {
            Serial.println("Error Occurred. Error #: " + String(Update.getError()));
            result += "Error #: " + String(Update.getError());
        }
    } else {
        Serial.println("Not enough space to begin OTA");
        result += "Not enough space for OTA";
    }

    if (deviceConnected) {
        sendOtaResult(result);
        delay(5000);
    }
}

void updateFromFS(fs::FS &fs) {
    File updateBin = fs.open("/update.bin");
    if (updateBin) {
        if (updateBin.isDirectory()) {
            Serial.println("Error, update.bin is not a file");
            updateBin.close();
            return;
        }
        size_t updateSize = updateBin.size();
        if (updateSize > 0) {
            Serial.println("Trying to start update");
            performUpdate(updateBin, updateSize);
        } else {
            Serial.println("Error, file is empty");
        }
        updateBin.close();
      // when finished remove the binary from spiffs to indicate end of the process
        Serial.println("Removing update file");
        fs.remove("/update.bin");

        rebootEspWithReason("Rebooting to complete OTA update");
    } else {
        Serial.println("Could not load update.bin from spiffs root");
    }
}

void initUpdaterBLE(BLEServer* server, BLEAdvertising *pAdvertising) {
    updaterService = server->createService(UPDATER_SERVICE_UUID);
    updaterTXChar = updaterService->createCharacteristic(UPDATER_TX_CHAR_UUID, BLECharacteristic::PROPERTY_NOTIFY);
    updaterRXChar = updaterService->createCharacteristic(UPDATER_RX_CHAR_UUID, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
    updaterRXChar->setCallbacks(new MyUpdaterCharacteristicCallbacks());
    updaterTXChar->setCallbacks(new MyUpdaterCharacteristicCallbacks());
    updaterTXChar->addDescriptor(new BLE2902());
    updaterTXChar->setNotifyProperty(true);
    updaterService->start();

    pAdvertising->addServiceUUID(UPDATER_SERVICE_UUID);
}

void initUpdaterFS() {
    #ifdef UPDATER__USE_SPIFFS
    if (!SPIFFS.begin(UPDATER__FORMAT_SPIFFS_IF_FAILED)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    #else
    if (!FFat.begin()) {
        Serial.println("FFat Mount Failed");
        if (UPDATER__FORMAT_FFAT_IF_FAILED) FFat.format();
        return;
    }
    #endif
}

void updaterHandleUpdateMode() {
    if (request) {
        uint8_t rq[] = {0xF1, (cur + 1) / 256, (cur + 1) % 256};
        updaterTXChar->setValue(rq, 3);
        updaterTXChar->notify();
        delay(50);
        request = false;
    }

    if (cur + 1 == parts) { // received complete file
        uint8_t com[] = {0xF2, (cur + 1) / 256, (cur + 1) % 256};
        updaterTXChar->setValue(com, 3);
        updaterTXChar->notify();
        delay(50);
        UPDATER__MODE = UPDATER__OTA_MODE;
    }

    if (writeFile) {
        if (!current) {
            writeBinary(UPDATER__FLASH, "/update.bin", updater, writeLen);
        } else {
            writeBinary(UPDATER__FLASH, "/update.bin", updater2, writeLen2);
        }
    }
}

void updaterHandleOTAMode() {
    if (writeFile) {
        if (!current) {
            writeBinary(UPDATER__FLASH, "/update.bin", updater, writeLen);
        } else {
            writeBinary(UPDATER__FLASH, "/update.bin", updater2, writeLen2);
        }
    }

    if (rParts == tParts) {
        Serial.println("Complete");
        delay(5000);
        updateFromFS(UPDATER__FLASH);
    } else {
        writeFile = true;
        Serial.println("Incomplete");
        Serial.print("Expected: ");
        Serial.print(tParts);
        Serial.print("Received: ");
        Serial.println(rParts);
        delay(2000);
    }
}
