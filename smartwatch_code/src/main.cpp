#include <Arduino.h>

#include "stepcounter.h"
#include "touchscreen.h"
#include "screens/watchface.h"
#include "ble.h"
#include "declarations.h"
#include "notifications.h"
#include "config.h"
#include "updater.h"

void print_system_info();

long phoneupdate = -5000;
long timeupdate = -15000;

void setup() {
    Serial.begin(115200);
    initConfig();
    pinMode(CASE_BUTTON, INPUT);
    pinMode(BATTERY_MEASURE, INPUT);
    // sc_setup();
    bt_setup();
    initUpdaterFS();
    touch_setup();
    screen_setup();
    // zero notification array
    for (int i=0;i<64;i++) {
        notifications[i] = NULL;
    }
    print_system_info();
}

int pastMode = UPDATER__MODE;

void loop() {
    if (pastMode != UPDATER__MODE && UPDATER__MODE != UPDATER__NORMAL_MODE) {
        screenUpdateInProgressMessage();
    }
    pastMode = UPDATER__MODE;
    switch (UPDATER__MODE) {
        case UPDATER__NORMAL_MODE:
            {
                if (millis() - phoneupdate > 5000) {
                    sendCommand("UPDATE_NOTIFS");
                    phoneupdate = millis();
                }
            
                if (millis() - timeupdate > 18000) {
                    sendCommand("SYNC_TIME");
                    sendCommand("SYNC_SPOTIFY");
                    timeupdate = millis();
                }
                sc_loop();
                bt_loop();
                touch_loop();
                screen_update();
            }
        break;
        case UPDATER__UPDATE_MODE:
            updaterHandleUpdateMode();
            break;
        case UPDATER__OTA_MODE:
            updaterHandleOTAMode();
            break;
    }
    
}

void print_system_info() {
    Serial.printf("CPU Information:\n\t");
    Serial.printf("Revision %d\n\t",  ESP.getChipRevision());
    Serial.printf("Model: %s\n\t", ESP.getChipModel());
    Serial.printf("Cores: %d\n", ESP.getChipCores());

    Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
}