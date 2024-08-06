#include "touchscreen.h"
#include "screens/watchface.h"
#include "screens/notificationscreen.h"
#include "screens/exercisescreen.h"
#include "screens/settingsscreen.h"
#include "screens/homescreen.h"
#include "screens/spotifyscreen.h"
#include "config.h"

#include <CST816S.h>
#include <TFT_eSPI.h>


#include "declarations.h"
#include "sleep.h"

TFT_eSPI tft = TFT_eSPI();
CST816S touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_IRQ); // sda, scl, rst, irq
Watchface wf = Watchface();
NotificationScreen ns = NotificationScreen();
ExerciseScreen es = ExerciseScreen();
SettingsScreen ss = SettingsScreen();
SpotifyScreen sp = SpotifyScreen();
Homescreen hs = Homescreen();
Screen* activeScreen = &hs;
TFT_eSprite wfsprite = TFT_eSprite(&tft);
TFT_eSprite nssprite = TFT_eSprite(&tft);
TFT_eSprite essprite = TFT_eSprite(&tft);
TFT_eSprite sssprite = TFT_eSprite(&tft);
TFT_eSprite spsprite = TFT_eSprite(&tft);
TFT_eSprite hssprite = TFT_eSprite(&tft);

bool isAnimating = false;

long timeOfLastInteraction = 0;
bool isAsleep = false;

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
        if (isAsleep) {
            // ignore the interaction from waking up the device
            isAsleep = false;
            timeOfLastInteraction = millis();
            setScreenBrightness(-1);
            return;
        }
        Serial.print(touch.gesture());
        Serial.print("\t");
        Serial.print(touch.data.points);
        Serial.print("\t");
        Serial.print(touch.data.event);
        Serial.print("\t");
        Serial.print(touch.data.x);
        Serial.print("\t");
        Serial.println(touch.data.y);
        if (millis() - timeOfLastInteraction < 200 || touch.gesture() == String("NONE")) {
            Serial.println("returning");
            return;
        }
        timeOfLastInteraction = millis();
        activeScreen->handleInteraction(touch.gesture(), touch.data.x, touch.data.y);
    }
    if (millis() - timeOfLastInteraction > 5000 && !isAsleep) { // change this number for time before sleep
        enableSleepMode();
        isAsleep = true;
    }
}

void screen_setup() {
    ledcSetup(0, 1000, 8); // setup pwm channel 0 with 1000 freq and 8 bit precision
    ledcAttachPin(SCREEN_BL, 0); // connect screen backlight pin to pwm 0
    setScreenBrightness(-1);
    tft.begin();
    tft.setRotation(0);
    tft.loadFont(FontLight14);
    tft.fillScreen(TFT_GREEN);
    wf.init(&wfsprite, 240, 240);
    ns.init(&nssprite, 240, 240);
    es.init(&essprite, 240, 240);
    ss.init(&sssprite, 240, 240);
    sp.init(&spsprite, 240, 240);
    hs.init(&hssprite, 240, 240);
}

int startMillis = 0;

void screen_update() {
    int mpf = millis() - startMillis;
    float fps = (float) ((float) 1000)/((float) mpf);
    // Serial.printf("FPS: %f\n", fps);
    startMillis = millis();
    activeScreen->render();
    // char fpss[8];
    // snprintf(fpss, sizeof(fps), "%.2f", fps);
    // tft.drawString(fpss, 120, 20);
}

void switchScr(Screen* new_screen) {
    activeScreen = new_screen;
}

void turnScreenOff() {
    tft.fillScreen(TFT_BLACK);
    ledcWrite(0, 0); // setting like this to avoid it getting saved
}

// val is from 1-255, -1 = load brightness from config
void setScreenBrightness(int val) {
    if (val == -1) { // load user set brightness and set it
        val = loadBrightness();
    }
    ledcWrite(0, val);
    saveBrightness(val);
}

int getScreenBrightness() {
    if (loadBrightness() != ledcRead(0)) {
        Serial.println("BRIGHTNESS PROBLEM!!!");
    }
    return ledcRead(0);
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

void recolorImage(const uint16_t* image, int w, int h, int repcol, int newcol, uint16_t* buf) {
    for (int i=0;i<h*w;i++) {
        if (image[i] == repcol) {
            buf[i] = newcol;
            continue;
        }
        buf[i] = image[i];
    }
}
