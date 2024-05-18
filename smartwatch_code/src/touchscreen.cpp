#include "touchscreen.h"
#include "screens/homescreen.h"
#include "screens/notificationscreen.h"

#include <CST816S.h>
#include <TFT_eSPI.h>


#include "declarations.h"

TFT_eSPI tft = TFT_eSPI();
CST816S touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_IRQ); // sda, scl, rst, irq
Homescreen hs = Homescreen();
NotificationScreen ns = NotificationScreen();
Screen* activeScreen = &hs;
TFT_eSprite hssprite = TFT_eSprite(&tft);
TFT_eSprite nssprite = TFT_eSprite(&tft);

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
        activeScreen->handleInteraction(touch.gesture(), touch.data.x, touch.data.y);
    }
}

void screen_setup() {
    tft.begin();
    tft.setRotation(0);
    tft.loadFont(FontLight14);
    tft.fillScreen(TFT_GREEN);
    hs.init(&hssprite, 240, 240);
    ns.init(&nssprite, 240, 240);
}

void screen_update() {
    activeScreen->render();
    delay(100);
}

void animateSwitch(AnimationSelect as, Screen* old_screen, Screen* new_screen) {
    // TODO: IMPLEMENT
    activeScreen = new_screen;
}
