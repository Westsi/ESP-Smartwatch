#include <Arduino.h>

#include "stepcounter.h"
#include "sdc.h"
#include "touchscreen.h"
#include "screens/homescreen.h"
#include "ble.h"
#include "declarations.h"

void print_system_info();

void setup() {
  Serial.begin(115200);
  // pinMode(CASE_BUTTON, INPUT);
  // sdc_setup();
  // sc_setup();
  print_system_info();
  bt_setup();
  touch_setup();
  screen_setup();
}

void loop() {
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