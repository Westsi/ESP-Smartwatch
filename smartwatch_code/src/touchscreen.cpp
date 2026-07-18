#include "touchscreen.h"
#include "screens/watchface.h"
#include "screens/notificationscreen.h"
#include "screens/exercisescreen.h"
#include "screens/settingsscreen.h"
#include "screens/homescreen.h"
#include "screens/spotifyscreen.h"
#include "screens/blackjackscreen.h"
#include "screens/stopwatch.h"
#include "config.h"

#include <CST816S.h>
#include <TFT_eSPI.h>
#include <math.h>


#include "declarations.h"
#include "sleep.h"

/*
TODO: multicore and partial updates
*/

TFT_eSPI tft = TFT_eSPI();
CST816S touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_IRQ); // sda, scl, rst, irq
Watchface wf = Watchface();
NotificationScreen ns = NotificationScreen();
ExerciseScreen es = ExerciseScreen();
SettingsScreen ss = SettingsScreen();
SpotifyScreen sp = SpotifyScreen();
Homescreen hs = Homescreen();
BlackjackScreen bs = BlackjackScreen();
Stopwatch sw = Stopwatch();
Screen* activeScreen = &wf;
TFT_eSprite wfsprite = TFT_eSprite(&tft);
TFT_eSprite nssprite = TFT_eSprite(&tft);
TFT_eSprite essprite = TFT_eSprite(&tft);
TFT_eSprite sssprite = TFT_eSprite(&tft);
TFT_eSprite spsprite = TFT_eSprite(&tft);
TFT_eSprite hssprite = TFT_eSprite(&tft);
TFT_eSprite bssprite = TFT_eSprite(&tft);
TFT_eSprite swsprite = TFT_eSprite(&tft);

struct DirtyRect {
    int x1 = SCREEN_W;
    int y1 = SCREEN_H;
    int x2 = -1;
    int y2 = -1;

    void include(int x, int y, int width, int height) {
        x1 = min(x1, x);
        y1 = min(y1, y);
        x2 = max(x2, x + width - 1);
        y2 = max(y2, y + height - 1);
    }

    bool valid() {
        return x2 >= x1 && y2 >= y1;
    }
};

float easeOutCubic(float t);
float easeInOutCubic(float t);
int getRadius(int frame, int totalFrames);
void drawCircularClip(TFT_eSprite* spr, int cx, int cy, int radius);

void generateSweepMask();
DirtyRect drawSweepFrame(uint8_t* frameBuf, uint8_t* newBuf, uint8_t prevProgress, uint8_t progress);

uint8_t* sweepMask;

const uint8_t bayer4[4][4] = {
    {0, 8, 2, 10},
    {12, 4, 14, 6},
    {3, 11, 1, 9},
    {15, 7, 13, 5}
};

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
    if (millis() - timeOfLastInteraction > 50000 && !isAsleep) { // change this number for time before sleep
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
    generateSweepMask();
    wf.init(&wfsprite, 240, 240);
    ns.init(&nssprite, 240, 240);
    es.init(&essprite, 240, 240);
    ss.init(&sssprite, 240, 240);
    sp.init(&spsprite, 240, 240);
    hs.init(&hssprite, 240, 240);
    bs.init(&bssprite, 240, 240);
    sw.init(&swsprite, 240, 240);
}

int startMillis = 0;

void screen_update() {
    int mpf = millis() - startMillis;
    // float fps = (float) ((float) 1000)/((float) mpf);
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

void screenUpdateInProgressMessage() {
    tft.fillScreen(TFT_BLACK);
    tft.drawString("Update in progress", 120, 120);
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

// TODO: cooler backgrounds than black, maybe a gradient like macos

float easeInOutCubic(float t) {
    // return 1.0f - powf(1.0f-t, 3);
    if (t <= 0.5f) {
        return 4 * t * t * t;
    }
    return 1 - powf(2-2*t, 3) / 2;
}

float easeOutCubic(float t) {
    return 1.0f - powf(1-t, 3);
}

int getRadius(int frame, int totalFrames) {
    // return (float)(((float)frame)/((float)totalFrames)) * 170.0f;

    const int maxRadius = 170;
    const int overshoot = 4;

    // for 80% of the animation reveal the new screen
    int revealFrames = totalFrames * 0.8f;

    if (frame < revealFrames) {
        float t = (float)frame / (revealFrames - 1);
        return easeInOutCubic(t) * maxRadius;
    }

    // last 20% is slight overshoot and return for settling
    float t = (float)(frame - revealFrames) / (totalFrames - revealFrames - 1);

    // go from 0 to 1 to 0
    float bounce = 0;
    if (t < 0.5f) {
        bounce = t * 2.0f;
    } else {
        bounce = 1.0f - (t - 0.5f) * 2.0f;
    }

    return maxRadius + bounce * overshoot;
}

void drawCircularClip(TFT_eSprite* spr, int cx, int cy, int radius) {
    int r2 = radius * radius;
    for (int y = max(0, cy - radius); y <= min(239, cy + radius); y++) {
        int dy = y - cy;
        int dx = sqrtf(r2 - dy * dy);

        int xStart = max(0, cx - dx);
        int width = min(239, cx + dx) - xStart + 1;

        if (width > 0) {
            spr->pushSprite(xStart, y, xStart, y, width, 1);
            // tft.fillRect(xStart, y, width, 1, TFT_RED);
        }
    }
}

void circleGrowAnimation(AnimationSelect as, Screen* old_screen, Screen* new_screen) {
    int stages = 15;
    int cx = 120;
    int cy = 120;
    long startMillis = millis();
    new_screen->update();
    TFT_eSprite* oss = old_screen->spr;
    TFT_eSprite* nss = new_screen->spr;
    oss->pushSprite(0, 0);
    for (int i=0;i<stages;i++) {
        int radius = getRadius(i, stages);
        Serial.printf("Stage is %d, radius is %d\n", i, radius);
        drawCircularClip(nss, cx, cy, radius);
        delay(30);
    }
    long timetaken = millis() - startMillis;
    float fps = (float) ((float) stages)/((float)((float)timetaken/1000));
    Serial.printf("Rendered %d animation frames in %d milliseconds, FPS: %s\n", stages, timetaken, String(std::to_string(fps).c_str()));
    activeScreen = new_screen;
}

void generateSweepMask() {
    sweepMask = (uint8_t*)heap_caps_malloc(SCREEN_W * SCREEN_H, MALLOC_CAP_SPIRAM);
    const int cx = SCREEN_W / 2;
    const int cy = SCREEN_H / 2;

    for (int y=0; y<SCREEN_H;y++) {
        for (int x=0;x<SCREEN_W;x++) {
            int dx = x - cx;
            int dy = cy-y; // invert so 0 degrees is at the top

            float angle = atan2f(dx, dy);

            if (angle < 0) {
                angle += 2 * PI;
            }

            sweepMask[y * SCREEN_W + x] = (uint8_t)(angle * 255.0f / (2 * PI));
        }
    }
}

DirtyRect drawSweepFrame(uint8_t* frameBuf, uint8_t* newBuf, uint8_t prevProgress, uint8_t progress) {
    uint8_t band = 15; // width of dither

    DirtyRect dirty;

    for (int y=0;y<SCREEN_H;y++) {
        int runStart = -1;
        for (int x=0;x<SCREEN_W;x++) {
            uint8_t a = sweepMask[y * SCREEN_W + x];
            bool draw = false;

            if (a > prevProgress && a <= progress) {
                draw = true;
            } else if ((uint8_t)(a-progress) < band) {
                // dither transition
                uint8_t threshold = bayer4[y&3][x&3] * band / 16;

                if ((uint8_t)(a-progress) < threshold) {
                    draw = true;
                }
            }

            if (draw && runStart < 0) {
                runStart = x;
            }

            if ((!draw || x == SCREEN_W - 1) && runStart >= 0) {
                int end = draw ? x : x - 1;
                memcpy(
                    frameBuf + y * SCREEN_W + runStart, 
                    newBuf + y * SCREEN_W + runStart,
                    end - runStart + 1
                );
                dirty.include(runStart, y, end - runStart + 1, 1);
                runStart = -1;
            }
        }
    }
    return dirty;
}

void sweepAnimation(AnimationSelect as, Screen* old_screen, Screen* new_screen) {
    int stages = 18;
    new_screen->update();
    TFT_eSprite* oss = old_screen->spr;
    TFT_eSprite* nss = new_screen->spr;
    oss->pushSprite(0, 0);
    TFT_eSprite frameSpr = TFT_eSprite(&tft);
    frameSpr.setColorDepth(8);
    frameSpr.createSprite(240, 240);

    uint8_t* frame = (uint8_t*)frameSpr.getPointer();
    uint8_t* oldBuf = (uint8_t*)old_screen->spr->getPointer();
    uint8_t* newBuf = (uint8_t*)new_screen->spr->getPointer();

    int previousProg = 0;

    memcpy(frame, oldBuf, SCREEN_W * SCREEN_H);
    long startMillis = millis();
    for (int i=0;i<stages;i++) {
        float t = (float) i / (stages-1);
        uint8_t angle = easeOutCubic(t) * 255;
        uint32_t t1 = micros();
        DirtyRect d = drawSweepFrame(frame, newBuf, previousProg, angle);
        previousProg = angle;
        uint32_t t2 = micros();
        if (d.valid()) {
            Serial.printf("dirty %dx%d\n", d.x2-d.x1+1, d.y2-d.y1+1);
            frameSpr.pushSprite(d.x1, d.y1, d.x1, d.y1, d.x2 - d.x1 + 1, d.y2 - d.y1 + 1);
        }
        // frameSpr.pushSprite(0, 0);
        uint32_t t3 = micros();
        Serial.printf("frame %d, angle %d: render %lu us, push %lu us\n", i, angle, t2 - t1, t3 - t2);
        delay(80);
    }
    activeScreen = new_screen;
    long timetaken = millis() - startMillis;
    float fps = (float) ((float) stages)/((float)((float)timetaken/1000));
    Serial.printf("Rendered %d animation frames in %d milliseconds, FPS: %s\n", stages, timetaken, String(std::to_string(fps).c_str()));
}

void animateSwitch(AnimationSelect as, Screen* old_screen, Screen* new_screen) {
    int stages = 15; // does animation actually look better?
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
                oss->resetViewport();
            }
            long timetaken = millis() - startMillis;
            float fps = (float) ((float) stages)/((float)((float)timetaken/1000));
            Serial.printf("Rendered %d animation frames in %d milliseconds, FPS: %s\n", stages, timetaken, String(std::to_string(fps).c_str()));
            break;
        }
    }
    
    activeScreen = new_screen;
}

void recolorImage(const uint16_t* image, int w, int h, int newcol, uint16_t* buf) {
    for (int i=0;i<h*w;i++) {
        if (image[i] != 0) {
            buf[i] = newcol;
            continue;
        }
        buf[i] = image[i];
    }
}
