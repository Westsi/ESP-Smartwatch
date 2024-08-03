#include "hardware_interface.h"
#include "declarations.h"

#define BATT_MIN 3.3
#define BATT_MAX 4.2

uint8_t getBatteryPercentage() {
    // TODO: implement percentages based off lipo discharge curves
    // https://electronics.stackexchange.com/questions/435837/calculate-battery-percentage-on-lipo-battery
    // reasonably accurate formula
    float v = 123.0 - (123.0 / pow(1.0 + pow(getBatteryVoltage() / 3.7, 80), 0.165));
    return (uint8_t) v;
}

float getBatteryVoltage() {
    float battv = (float)analogRead(BATTERY_MEASURE);
    battv = battv / 4095; // adc "units" aka how many 4095ths of max voltage/2 we are seeing
    battv = battv * BATT_MAX;
    battv = battv * 2;

    return battv;
} 