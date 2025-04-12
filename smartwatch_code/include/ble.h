#ifndef BLE_CONN_H_
#define BLE_CONN_H_

#include <Arduino.h>
#include <string>

void bt_setup();
void bt_loop();

void writeCharacteristics();
void writeDebugInfo();

void updateTime();
void handleReceivedData(std::string data);

void sendCommand(std::string cmd);

extern bool deviceConnected;

#endif