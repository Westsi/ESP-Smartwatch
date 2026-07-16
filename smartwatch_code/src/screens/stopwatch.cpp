#include "screens/stopwatch.h"
#include "touchscreen.h"
#include "time.h"
#include "declarations.h"
#include "ble.h"
#include <TFT_eSPI.h>

void swFullScreenHandler(String gesture, int x, int y);

unsigned long millisStart = 0;
unsigned long microsStart = 0;


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
    // use micros for stuff less than 60 seconds, millis for anything over that
    int elapsed = 0;
    int tmul = 1000; // 1000 ms in 1s
    unsigned long mSElapsed = (millis() - millisStart);
    if (mSElapsed > 70) {
        elapsed = mSElapsed;
    } else {
        elapsed = (micros() - microsStart);
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
    // TODO: make sure always to show leading 0 for m and s, and leading 00 for ms and us
    char tb[32];
    if (showHMS) {
        sprintf(tb, "%d:%d:%d", h, m, s);
    } else if (showMSMs) {
        sprintf(tb, "%d:%d.%d", m, s, ms);
    } else if (showSMsUs) {
        sprintf(tb, "%d.%d %du", s, ms, us);
    }
    
    spr->unloadFont();
    spr->loadFont(FontRubik64);
    spr->drawString(tb, 120, 80);
    spr->unloadFont();
    spr->loadFont(FontLight20);

    // TODO: add start/stop/reset button
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

    }
    else if (gesture == "SINGLE CLICK") {
        
    }
    else if (gesture == "DOUBLE CLICK") {

    }
    else if (gesture == "LONG PRESS") {

    }
}