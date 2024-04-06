#include <Arduino.h>

#include "stepcounter.h"
#include "sdc.h"
#include "touchscreen.h"
#include "phone_connection.h"
#include "devicepins.h"

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
  bt_loop();
}