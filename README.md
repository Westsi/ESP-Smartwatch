![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/Westsi/ESP-Smartwatch/pio.yml?style=for-the-badge&logo=github)
# ESP32 Based Smartwatch
A smartwatch with modern features such as notifications and Spotify sync, watch faces that can be customised, a 240x240 round LCD screen, and a companion app connected with a custom BLE profile.


## Features
- Custom BLE Profile allowing for
  - Notification Sync
  - Time Sync
  - Spotify Sync
  - Exercise Sync
- Auto Sleep Timeout to save battery
- Adjustable Brightness
- Semi-customizeable Watch Faces
- Rudimentary Exercise Tracker
- Code for wireless (OTA) updates (most code from [fbiego/ESP32_BLE_OTA_Arduino](https://github.com/fbiego/ESP32_BLE_OTA_Arduino))

## Prototype 1
![alt text](PXL_20250407_110429820.MP.jpg "Prototype 1 Assembly")

[Demo Video (youtube.com)](https://www.youtube.com/watch?v=f3tLT5q3JZY)

## TODO
- [ ] Fix OTA updates over BLE
- [ ] Make weather sync as well
- [x] Put it all together
- [ ] Calibrate battery level code
- [ ] Make Exercise Tracker work properly with accelerometer
- [ ] Saveable Preferences
