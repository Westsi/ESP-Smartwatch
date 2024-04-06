#include "stepcounter.h"
#include "devicepins.h"

#include <Wire.h>
#include "MPU6050_tockn.h"

MPU6050 mpu(Wire);

int16_t aX, aY, aZ, gX, gY, gZ;

long timer = 0;

void sc_setup() {
    Wire.begin(MPU_SDA, MPU_SCL);
    mpu.begin();
    mpu.calcGyroOffsets(true);
}

void sc_loop() {
    if (millis() - timer > 1000) {
        sc_read();
        sc_write_serial();
        timer = millis();
    }
}

void sc_read() {
    mpu.update();
    aX = mpu.getRawAccX();
    aY = mpu.getRawAccY();
    aZ = mpu.getRawAccZ();
    gX = mpu.getRawGyroX();
    gY = mpu.getRawGyroY();
    gZ = mpu.getRawGyroZ();
}

void sc_write_serial() {
    Serial.print(aX);
    Serial.print(",");
    Serial.print(aY);
    Serial.print(",");
    Serial.print(aZ);
    Serial.print(",");
    Serial.print(gX);
    Serial.print(",");
    Serial.print(gY);
    Serial.print(",");
    Serial.println(gZ);
}
