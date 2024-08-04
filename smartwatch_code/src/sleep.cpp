#include "sleep.h"
#include "touchscreen.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "esp_bt_main.h"

void enableSleepMode() {
    // configure to wakeup when touch irq line goes low - has been pressed
    gpio_wakeup_enable(GPIO_NUM_25, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();
    turnScreenOff();
    
    esp_light_sleep_start();


    // esp_sleep_enable_timer_wakeup(60*1000000); // 60 seconds in microseconds
    // esp_light_sleep_start();
    // esp_sleep_wakeup_cause_t swct = esp_sleep_get_wakeup_cause();
    // esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    // Serial.println(swct);
    // delay(200);
    // if (swct == ESP_SLEEP_WAKEUP_TIMER) {
    //     /*
    //     If it woke up from timer, put into deep sleep
    //     */
    //     // esp_bluedroid_disable - esp_bt_controller_disable - esp_wifi_stop
    //     Serial.println("entering deep sleep");

    //     BLEDevice::stopAdvertising();
    //     BLEDevice::deinit(true);
    //     esp_bt_controller_disable();
    //     esp_bt_controller_deinit();
    //     esp_bluedroid_disable();
    //     esp_bluedroid_deinit();

    //     esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, 0);
    //     esp_deep_sleep_start();
    // }


    // if (err != ESP_OK) {
    //     Serial.println("HALP NOT GOOD");
    // }
    Serial.println("Sleep ended");
}

void disableSleepMode() {
    // idk
}
