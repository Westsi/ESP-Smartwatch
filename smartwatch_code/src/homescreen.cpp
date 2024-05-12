#include "screens/homescreen.h"
#include "touchscreen.h"
#include "time.h"
#include "declarations.h"
#include <TFT_eSPI.h>

void testHandler(String gesture, int x, int y);

void Homescreen::init(TFT_eSprite* spr, int width, int height) {
    iHLen = 0;
    this->spr = spr;
    spr->loadFont(FontLight20);
    spr->setColorDepth(8);
    spr->createSprite(width, height);
    spr->fillSprite(TFT_XON_BLUE);
    spr->setTextColor(TFT_BLACK, TFT_XON_BLUE, true);
    spr->setTextDatum(MC_DATUM);
    spr->setTextWrap(true);
    this->registerInteractionHandler(testHandler, 0, 240, 0, 240);
}

void Homescreen::update() {
    time_t now;
    char hms_buf[64];
    char dmy_buf[64];
    struct tm timeinfo;

    time(&now);
    setenv("TZ", "GMT", 1);
    tzset();

    localtime_r(&now, &timeinfo);
    strftime(hms_buf, sizeof(hms_buf), "%H:%M:%S", &timeinfo);
    strftime(dmy_buf, sizeof(dmy_buf), "%a %d/%m/%y", &timeinfo);
    // Serial.println(strftime_buf);
    spr->drawString(hms_buf, 120, 120);
    spr->drawString(dmy_buf, 120, 140);
}

void Homescreen::render() {
    this->update();
    spr->pushSprite(0, 0);
}

void Homescreen::registerInteractionHandler(InteractionCallback callback, int minx, int maxx, int miny, int maxy) {
    InteractionHandler ih = InteractionHandler{.callback = callback, .minx = minx, .maxx = maxx, .miny = miny, .maxy = maxy};
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void Homescreen::registerInteractionHandler(InteractionHandler ih) {
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void Homescreen::handleInteraction(String gesture, int x, int y) {
    Serial.printf("%s interaction with homescreen at %d %d\n", gesture, x, y);
    // loop through interaction handlers and check if gesture is within their bounds
    for (auto ih : interactionHandlers) {
        if (x > ih.minx && x < ih.maxx && y > ih.miny && y < ih.maxy) {
            Serial.println("Found gesture handler");
            ih.callback(gesture, x, y);
        }
    }
}

void testHandler(String gesture, int x, int y) {
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