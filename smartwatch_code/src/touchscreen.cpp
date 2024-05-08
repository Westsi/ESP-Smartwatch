#include "touchscreen.h"

#include <CST816S.h>
#include <TFT_eSPI.h>
#include "declarations.h"
#include "fonts/NotoSansBold36.h"
#define AA_FONT_LARGE NotoSansBold36

TFT_eSPI tft = TFT_eSPI();
CST816S touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_IRQ); // sda, scl, rst, irq

void touch_setup() {
    touch.begin();

    Serial.print(touch.data.version);
    Serial.print("\t");
    Serial.print(touch.data.versionInfo[0]);
    Serial.print("-");
    Serial.print(touch.data.versionInfo[1]);
    Serial.print("-");
    Serial.println(touch.data.versionInfo[2]);
}

void touch_loop() {
        if (touch.available()) {
        Serial.print(touch.gesture());
        Serial.print("\t");
        Serial.print(touch.data.points);
        Serial.print("\t");
        Serial.print(touch.data.event);
        Serial.print("\t");
        Serial.print(touch.data.x);
        Serial.print("\t");
        Serial.println(touch.data.y);
    }
}

void screen_setup() {
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
}

void screen_update() {
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.fillRoundRect(60, 100, 120, 40, 5, TFT_BLUE);
    int width = tft.drawString("Click me!", 70, 105);
    delay(100);
}

/*
TODO: Screen Management System

an array of Screens that have a setup method, an update method, an array of click methods at different coordinates
the screen update method just renders the current index into the screens array
sprites are *dynamically updated* - HOW?
*/
