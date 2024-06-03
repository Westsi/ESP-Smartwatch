#include "sleep.h"
#include "touchscreen.h"

void enableSleepMode() {
    // configure to wakeup when touch irq line goes low - has been pressed
    gpio_wakeup_enable(GPIO_NUM_26, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();
    turnScreenOff();
    // esp_err_t err = esp_light_sleep_start();
    delay(200);
    esp_light_sleep_start();
    delay(200);
    // if (err != ESP_OK) {
    //     Serial.println("HALP NOT GOOD");
    // }
    Serial.println("Sleep ended");
}

void disableSleepMode() {
    // idk
}
