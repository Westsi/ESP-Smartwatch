#include <Arduino.h>

#include "stepcounter.h"
#include "sdc.h"
#include "touchscreen.h"
#include "ble.h"
#include "declarations.h"

void setup() {
  Serial.begin(115200);
  // pinMode(CASE_BUTTON, INPUT);
  // sdc_setup();
  // sc_setup();
  // touch_setup();
  // screen_setup();
  bt_setup();
  
}

void loop() {
  sc_loop();
  bt_loop();
}