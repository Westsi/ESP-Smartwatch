#include "hardware_interface.h"

uint8_t getBatteryPercentage() {
    static int percentage = 100;
    static long timer = 0;
    if (millis() - timer > 10000) {
        timer = millis();
        percentage -= 5;
        if (percentage < 0) {
            percentage = 100;
        }
    }
    return (uint8_t) percentage;
}