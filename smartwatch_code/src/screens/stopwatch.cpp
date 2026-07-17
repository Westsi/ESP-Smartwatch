#include "screens/stopwatch.h"
#include "screens/homescreen.h"
#include "touchscreen.h"
#include "time.h"
#include "declarations.h"
#include "ble.h"
#include <TFT_eSPI.h>

void swFullScreenHandler(String gesture, int x, int y);

void drawButtons(TFT_eSprite* spr);
void handleStartStop();
void handleLapReset();

unsigned long cumMillis = 0;
unsigned long cumMicros = 0;

unsigned long millisStart = 0;
unsigned long microsStart = 0;
bool isRunning = false;


void Stopwatch::init(TFT_eSprite* spr, int width, int height) {
    // setup
    iHLen = 0;
    this->spr = spr;
    spr->loadFont(FontLight20);
    spr->setColorDepth(8);
    spr->createSprite(width, height);
    spr->fillSprite(TFT_BLACK);
    spr->setTextColor(TFT_WHITE, TFT_BLACK, true);
    spr->setTextDatum(MC_DATUM);
    spr->setTextWrap(true);
    registerInteractionHandler(swFullScreenHandler, 0, 240, 0, 240);
}

void Stopwatch::update() {
    spr->fillScreen(TFT_BLACK);
    char tb[32];
    int elapsed = 0;
    int tmul = 1000; // 1000 ms in 1s
    unsigned long mSElapsed = 0;
    unsigned long uSElapsed = 0;
    if (isRunning) {
        // use micros for stuff less than 60 seconds, millis for anything over that
        mSElapsed = (millis() - millisStart) + cumMillis;
        uSElapsed = (micros() - microsStart) + cumMicros;
    } else {
        mSElapsed = cumMillis;
        uSElapsed = cumMicros;
    }

    if (mSElapsed > 60 * 1000) {
        elapsed = mSElapsed;
    } else {
        elapsed = uSElapsed;
        tmul = 1000 * 1000; // 1000000 us in 1s
    }
    /*
    If hours, show h, m, s
    If minutes, show m, s, ms
    If seconds, show s, ms, us
    */
    
    int h = elapsed / (3600 * tmul);
    elapsed = elapsed % (3600 * tmul);
    int m = elapsed / (60 * tmul);
    elapsed = elapsed % (60 * tmul);
    int s = elapsed / tmul;
    elapsed = elapsed % tmul;
    int ms, us;
    if (tmul == 1000) {
        ms = elapsed;
        us = -1;
    } else {
        ms = elapsed / 1000;
        us = elapsed % 1000;
    }

    bool showHMS = false;
    bool showMSMs = false;
    bool showSMsUs = false;
    if (h > 0) {
        showHMS = true;
    } else if (m > 0) {
        showMSMs = true;
    } else {
        showSMsUs = true;
    }
    
    if (showHMS) {
        sprintf(tb, "%d:%02d:%02d", h, m, s);
    } else if (showMSMs) {
        sprintf(tb, "%d:%02d.%03d", m, s, ms);
    } else if (showSMsUs) {
        sprintf(tb, "%d.%03d %03du", s, ms, us);
    }
    
    spr->unloadFont();
    spr->loadFont(FontRubik28);
    spr->setTextColor(TFT_XON_BLUE, TFT_BLACK, true);
    spr->drawString(tb, 120, 80);
    spr->unloadFont();
    spr->loadFont(FontLight20);
    spr->setTextColor(TFT_WHITE, TFT_BLACK, true);

    drawButtons(spr);
    // TODO: down the line use side button for start/stop
}

void Stopwatch::render() {
    this->update();
    spr->pushSprite(0, 0);
}

void Stopwatch::registerInteractionHandler(InteractionCallback callback, int minx, int maxx, int miny, int maxy) {
    InteractionHandler ih = InteractionHandler{.callback = callback, .minx = minx, .maxx = maxx, .miny = miny, .maxy = maxy};
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void Stopwatch::registerInteractionHandler(InteractionHandler ih) {
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void Stopwatch::handleInteraction(String gesture, int x, int y) {
    Serial.printf("%s interaction with watchface at %d %d\n", gesture, x, y);
    // loop through interaction handlers and check if gesture is within their bounds
    for (auto ih : interactionHandlers) {
        if (x > ih.minx && x < ih.maxx && y > ih.miny && y < ih.maxy) {
            Serial.println("Found gesture handler");
            ih.callback(gesture, x, y);
            break;
        }
    }
}

void swFullScreenHandler(String gesture, int x, int y) {
    Serial.println("Test handler called");
    if (gesture == "SWIPE DOWN") {
        
    }
    else if (gesture == "SWIPE UP") {

    }
    else if (gesture == "SWIPE LEFT") {

    }
    else if (gesture == "SWIPE RIGHT") {
        switchScr(&hs);
    }
    else if (gesture == "SINGLE CLICK") {
        // stop/start x: 80-160, y: 130-160
        // lap/reset x: 80-160, y: 170-200
        if ((x >= 80) && (x <= 160)) {
            if ((y >= 130) && (y <= 160)) {
                handleStartStop();
            } else if ((y >= 170) && (y <= 220)) {
                handleLapReset();
            }
        }
    }
    else if (gesture == "DOUBLE CLICK") {

    }
    else if (gesture == "LONG PRESS") {

    }
}

void drawButtons(TFT_eSprite* spr) {
    spr->fillSmoothRoundRect(80, 130, 80, 30, 10, TFT_XON_BLUE);
    spr->setTextColor(TFT_WHITE, TFT_XON_BLUE, true);
    if (isRunning) {
        spr->drawString("Stop", 120, 145);
    } else {
        spr->drawString("Start", 120, 145);
    }
    spr->setTextColor(TFT_WHITE, TFT_BLACK, true);

    spr->fillSmoothRoundRect(80, 170, 80, 30, 10, TFT_XON_DARK_BLUE);
    spr->setTextColor(TFT_WHITE, TFT_XON_DARK_BLUE, true);
    if (isRunning) {
        spr->drawString("Lap", 120, 185);
    } else {
        spr->drawString("Reset", 120, 185);
    }
    spr->setTextColor(TFT_WHITE, TFT_BLACK, true);
}

void handleStartStop() {
    if (isRunning) {
        // stop
        cumMillis += (millis() - millisStart);
        cumMicros += (micros() - microsStart);
        millisStart = millis();
        microsStart = micros();
        isRunning = false;
    } else {
        // start
        millisStart = millis();
        microsStart = micros();
        isRunning = true;
    }
}

void handleLapReset() {
    if (isRunning) {
        // lap
        // TODO: implement
    } else {
        // reset
        cumMillis = 0;
        cumMicros = 0;
    }
}