#include "touchscreen.h"
#include "screens/homescreen.h"

#include <CST816S.h>
#include <TFT_eSPI.h>

#include "fonts/NotoSansBold36.h"
#define AA_FONT_LARGE NotoSansBold36

#include "declarations.h"

TFT_eSPI tft = TFT_eSPI();
CST816S touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_IRQ); // sda, scl, rst, irq
Homescreen activeScreen = Homescreen();
TFT_eSprite sprite = TFT_eSprite(&tft);

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
        activeScreen.handleInteraction(touch.gesture(), touch.data.x, touch.data.y);
    }
}

void screen_setup() {
    tft.begin();
    tft.setRotation(0);
    tft.loadFont(AA_FONT_LARGE);
    tft.fillScreen(TFT_GREEN);
    activeScreen.init(&sprite, 240, 240);
}

void screen_update() {
    activeScreen.render();
    delay(100);
}

/*
TODO: Screen Management System

an array of Screens that have a setup method, an update method, an array of interaction handlers at different coordinates or overall
    e.g. swipe left on any screen is back etc.
the screen update method just renders the current index into the screens array
sprites are *dynamically updated* - HOW?

TODO: antialiasing with fonts - default with sprites?
LOOK AT ARDUINO IDE OPEN FILE
*/
