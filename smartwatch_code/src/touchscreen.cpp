#include "touchscreen.h"
#include "screens/watchface.h"
#include "screens/notificationscreen.h"
#include "screens/exercisescreen.h"

#include <CST816S.h>
#include <TFT_eSPI.h>


#include "declarations.h"

TFT_eSPI tft = TFT_eSPI();
CST816S touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_IRQ); // sda, scl, rst, irq
Watchface wf = Watchface();
NotificationScreen ns = NotificationScreen();
ExerciseScreen es = ExerciseScreen();
Screen* activeScreen = &wf;
TFT_eSprite wfsprite = TFT_eSprite(&tft);
TFT_eSprite nssprite = TFT_eSprite(&tft);
TFT_eSprite essprite = TFT_eSprite(&tft);

bool isAnimating = false;

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
    wf.init(&wfsprite, 240, 240);
    ns.init(&nssprite, 240, 240);
    es.init(&essprite, 240, 240);
}

int startMillis = 0;

void screen_update() {
    int mpf = millis() - startMillis;
    float fps = (float) ((float) 1000)/((float) mpf);
    Serial.printf("FPS: %f\n", fps);
    startMillis = millis();
    activeScreen->render();
}

void switchScr(Screen* new_screen) {
    activeScreen = new_screen;
}

void animateSwitch(AnimationSelect as, Screen* old_screen, Screen* new_screen) {
    int stages = 30; // does animation actually look better?
    int step = 240/stages;
    new_screen->update();
    switch (as) {
        case ANIMATE_IN_FROM_RIGHT:
        {
            long startMillis = millis();
            for (int i=0;i<stages;i++) {
                TFT_eSprite* oss = old_screen->spr;
                TFT_eSprite* nss = new_screen->spr;
                int os_x = (step*i);
                int os_width = 240 - (step*i);

                int ns_x = 0;
                int ns_width = (step*i);
                oss->setViewport(os_x, 0, os_width, 240);
                oss->pushSprite(0, 0, os_x, 0, os_width, 240);
                nss->setViewport(0, ns_x, 0, ns_width, 240);
                nss->pushSprite(os_width, 0, ns_x, 0, ns_width, 240);
                oss->resetViewport();
                nss->resetViewport();
            }
            long timetaken = millis() - startMillis;
            float fps = (float) ((float) stages)/((float)((float)timetaken/1000));
            Serial.printf("Rendered %d animation frames in %d milliseconds, FPS: %s\n", stages, timetaken, String(std::to_string(fps).c_str()));
            break;
        }
        case ANIMATE_IN_FROM_LEFT:      
        {
            long startMillis = millis();
            for (int i=stages;i>0;i--) {
                TFT_eSprite* oss = old_screen->spr;
                TFT_eSprite* nss = new_screen->spr;
                int ns_x = (step*i);
                int ns_width = 240 - (step*i);

                int os_x = 0;
                int os_width = (step*i);
                nss->setViewport(ns_x, 0, ns_width, 240);
                nss->pushSprite(0, 0, ns_x, 0, ns_width, 240);
                oss->setViewport(0, os_x, 0, os_width, 240);
                oss->pushSprite(ns_width, 0, os_x, 0, os_width, 240);
                nss->resetViewport();
                nss->resetViewport();
            }
            long timetaken = millis() - startMillis;
            float fps = (float) ((float) stages)/((float)((float)timetaken/1000));
            Serial.printf("Rendered %d animation frames in %d milliseconds, FPS: %s\n", stages, timetaken, String(std::to_string(fps).c_str()));
            break;
        }
    }
    
    activeScreen = new_screen;
}
