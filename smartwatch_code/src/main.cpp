#include <Arduino.h>

#include "stepcounter.h"
#include "touchscreen.h"
#include "screens/watchface.h"
#include "ble.h"
#include "declarations.h"
#include "notifications.h"
#include "config.h"

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
    touch_setup();
    screen_setup();
    // zero notification array
    for (int i=0;i<64;i++) {
        notifications[i] = NULL;
    }
    print_system_info();
}

void loop() {
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

void print_system_info() {
    Serial.printf("CPU Information:\n\t");
    Serial.printf("Revision %d\n\t",  ESP.getChipRevision());
    Serial.printf("Model: %s\n\t", ESP.getChipModel());
    Serial.printf("Cores: %d\n", ESP.getChipCores());

    Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
}