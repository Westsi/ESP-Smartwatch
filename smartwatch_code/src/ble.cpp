#include "ble.h"

#include "declarations.h"
#include "stepcounter.h"
#include "hardware_interface.h"
#include "notifications.h"
#include "screens/spotifyscreen.h"
#include "updater.h"

#include <Arduino.h>
#include <string>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <esp_gatt_common_api.h>

#define STEP_SERVICE_UUID "00001809-0000-1000-8000-00805F9B34FB"
#define STEP_CHAR_UUID "00002B40-0000-1000-8000-00805F9B34FB"           // integer, updates

#define BATTERY_SERVICE_UUID "0000180F-0000-1000-8000-00805F9B34FB"
#define BATTERY_LEVEL_CHAR_UUID "00002A19-0000-1000-8000-00805F9B34FB"  // integer, updates

#define DEVICE_INFO_SERVICE_UUID "0000180A-0000-1000-8000-00805F9B34FB"
#define MANU_NAME_CHAR_UUID "00002A29-0000-1000-8000-00805F9B34FB"      // string, SOAFAI (set once and forget about it)
#define MODEL_NUM_CHAR_UUID "00002A24-0000-1000-8000-00805F9B34FB"      // string, SOAFAI
#define FIRMWARE_REV_CHAR_UUID "00002A26-0000-1000-8000-00805F9B34FB"   // string, SOAFAI
#define HARDWARE_REV_CHAR_UUID "00002A27-0000-1000-8000-00805F9B34FB"   // string, SOAFAI
#define SOFTWARE_REV_CHAR_UUID "00002A28-0000-1000-8000-00805F9B34FB"   // string, SOAFAI
#define SYSTEM_ID_CHAR_UUID "00002A23-0000-1000-8000-00805F9B34FB"      // string works, SOAFAI

#define COMMAND_SERVICE_UUID "0dd0c28b-d173-43bf-9dce-f2446591366d"
#define COMMAND_CHAR_UUID "fed4ded3-97f1-44ca-b7a3-116cf78d9e77"

#define TIME_SERVICE_UUID "00001805-0000-1000-8000-00805F9B34FB"
#define TIME_CHAR_UUID "00002A2B-0000-1000-8000-00805F9B34FB"
#define TIME_ZONE_CHAR_UUID "00002A0E-0000-1000-8000-00805F9B34FB"

BLEServer* server = NULL;

BLEService* batteryService = NULL;
BLECharacteristic* batteryLevel = NULL;

BLEService* stepService = NULL;
BLECharacteristic* stepCount = NULL;

BLEService* deviceInfoService = NULL;

BLEService* commandService = NULL;
BLECharacteristic* command = NULL;

BLEService* timeService = NULL;
BLECharacteristic* timeCharac = NULL;
BLECharacteristic* timeZoneCharac = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;

bool awaitingResponse = false;
bool prevWaitStat = false;

std::string responseStr;

std::string connAddr = "!!";
uint16_t connectionID = 0;

long ble_timer = 0;

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

class MyCommandCharacteristicCallbacks: public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic* commandChar) {
        std::string cmd = commandChar->getValue();
        // WHY DOES THIS NOT WORK
        // Serial.println(cmd.c_str());
        if (cmd == "UPDATE_TIME") {
            updateTime();
         } else {
            if (awaitingResponse) {
                handleReceivedData(std::string(cmd.c_str()));
            }
        }
    }

    void onRead(BLECharacteristic* characteristic) {
        Serial.println("Value read from characteristic");
    }
};

void bt_setup() {
    Serial.println("Starting BLE work!");
    BLEDevice::init("XSWatch");
    esp_err_t err = esp_ble_gatt_set_local_mtu(256);
    err = esp_ble_gap_config_local_icon(ESP_BLE_APPEARANCE_GENERIC_WATCH);

    server = BLEDevice::createServer();
    server->setCallbacks(new MyServerCallbacks());
    
    batteryService = server->createService(BATTERY_SERVICE_UUID);
    batteryLevel = batteryService->createCharacteristic(
        BATTERY_LEVEL_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    batteryLevel->addDescriptor(new BLE2902());

    stepService = server->createService(STEP_SERVICE_UUID);
    stepCount = stepService->createCharacteristic(
        STEP_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE
    );
    stepCount->addDescriptor(new BLE2902());

    deviceInfoService = server->createService(DEVICE_INFO_SERVICE_UUID);
    deviceInfoService->createCharacteristic(
        MANU_NAME_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_NOTIFY
    )
    ->setValue(MANU_NAME);
    deviceInfoService->createCharacteristic(
        MODEL_NUM_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_NOTIFY
    )
    ->setValue(MODEL_NUM);
    deviceInfoService->createCharacteristic(
        FIRMWARE_REV_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_NOTIFY
    )
    ->setValue(FIRMWARE_REV);
    deviceInfoService->createCharacteristic(
        HARDWARE_REV_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_NOTIFY
    )
    ->setValue(HARDWARE_REV);
    deviceInfoService->createCharacteristic(
        SOFTWARE_REV_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_NOTIFY
    )
    ->setValue(SOFTWARE_REV);
    deviceInfoService->createCharacteristic(
        SYSTEM_ID_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_NOTIFY
    )
    ->setValue(SYSTEM_ID);

    commandService = server->createService(COMMAND_SERVICE_UUID);
    command = commandService->createCharacteristic(
        COMMAND_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_WRITE  |
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE
    );
    command->addDescriptor(new BLE2902());
    command->setCallbacks(new MyCommandCharacteristicCallbacks());

    timeService = server->createService(TIME_SERVICE_UUID);
    timeCharac = timeService->createCharacteristic(
        TIME_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_WRITE  |
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE
    );
    timeCharac->addDescriptor(new BLE2902());

    timeZoneCharac = timeService->createCharacteristic(
        TIME_ZONE_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_WRITE  |
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE
    );
    timeZoneCharac->addDescriptor(new BLE2902());
    

    batteryService->start();
    stepService->start();
    deviceInfoService->start();
    commandService->start();
    timeService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    initUpdaterBLE(server, pAdvertising);
    pAdvertising->addServiceUUID(BATTERY_SERVICE_UUID);
    pAdvertising->addServiceUUID(DEVICE_INFO_SERVICE_UUID);
    pAdvertising->addServiceUUID(STEP_SERVICE_UUID);
    pAdvertising->addServiceUUID(COMMAND_SERVICE_UUID);
    pAdvertising->addServiceUUID(TIME_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->setAppearance(ESP_BLE_APPEARANCE_GENERIC_WATCH);
    BLEDevice::startAdvertising();
    Serial.println("BLE started");
}

void bt_loop() {
    if (deviceConnected && (millis() - ble_timer > 1000)) {
        ble_timer = millis();
        writeCharacteristics();
        writeDebugInfo();
        // printAllNotifications();
    }
    if (!deviceConnected && oldDeviceConnected) {
        delay(500);
        server->startAdvertising();
        Serial.println("Start advertising");
        oldDeviceConnected = deviceConnected;
    }
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}

void writeCharacteristics() {
    uint32_t steps = getSteps();
    stepCount->setValue(steps);
    stepCount->notify();
    uint8_t batteryperc = getBatteryPercentage();
    batteryLevel->setValue(&batteryperc, 1);
    batteryLevel->notify();
}

void writeDebugInfo() {
    connAddr = ((BLEClient*)server->getPeerDevices(true)[connectionID].peer_device)->getPeerAddress().toString();
    // Serial.print("Connected to ");
    // Serial.println(connAddr.c_str());
}

void updateTime() {
    std::string t = timeCharac->getValue();
    time_t tt = stoi(t);
    timeval tval;
    tval.tv_sec = tt;
    tval.tv_usec = 0;

    std::string z = timeZoneCharac->getValue();

    setenv("TZ", z.c_str(), 1);
    tzset();
    settimeofday(&tval, NULL);
}

void handleReceivedData(std::string data) {
    // Serial.printf("RECEIVED DATA: \n\t");
    // Serial.println(data.c_str());
    if (data == "END_SONG") {
        parseSpotify(responseStr);
        responseStr.clear();
        return;
    }
    if (data == "END_NOTIF") {
        Notification* notif = parseNotification(responseStr);
        updateNotifications(notif);
        responseStr.clear();
        return;
    } else if (data ==  "END_ALL") {
        responseStr.clear();
        return;
    }
    responseStr += data;
}

void sendCommand(std::string cmd) {
    if (cmd.length() >= 256) {
        Serial.println("Command too long!");
    }
    command->setValue(cmd);
    command->notify();
    Serial.println("Sent command");
    awaitingResponse = true;
}