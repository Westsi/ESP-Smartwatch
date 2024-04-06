#include "phone_connection.h"

#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void bt_setup() {
    SerialBT.begin("XSWatch");
    Serial.println("The device started, now you can pair it with bluetooth!");
}

void bt_loop() {
    if (Serial.available()) {
        SerialBT.write(Serial.read());
    }
    if (SerialBT.available()) {
        Serial.write(SerialBT.read());
    }
    delay(20);
}