#ifndef STEP_COUNTER_H_
#define STEP_COUNTER_H_

#include <Wire.h>
#include "MPU6050_tockn.h"
#include <Arduino.h>

void sc_setup();
void sc_loop();
void sc_read();
void sc_write_serial();

uint32_t getSteps();

#endif

