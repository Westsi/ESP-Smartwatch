#include "screens/templatescreen.h"
#include "touchscreen.h"
#include "time.h"
#include "declarations.h"
#include "ble.h"
#include <TFT_eSPI.h>

void snFullScreenHandler(String gesture, int x, int y);


void SCREENNAME::init(TFT_eSprite* spr, int width, int height) {
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
    registerInteractionHandler(snFullScreenHandler, 0, 240, 0, 240);

    // clock face
    spr->fillCircle(120, 120, 118, TFT_BLACK);
}

void SCREENNAME::update() {
    
}

void SCREENNAME::render() {
    this->update();
    spr->pushSprite(0, 0);
}

void SCREENNAME::registerInteractionHandler(InteractionCallback callback, int minx, int maxx, int miny, int maxy) {
    InteractionHandler ih = InteractionHandler{.callback = callback, .minx = minx, .maxx = maxx, .miny = miny, .maxy = maxy};
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void SCREENNAME::registerInteractionHandler(InteractionHandler ih) {
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void SCREENNAME::handleInteraction(String gesture, int x, int y) {
    Serial.printf("%s interaction with watchface at %d %d\n", gesture, x, y);
    // loop through interaction handlers and check if gesture is within their bounds
    for (auto ih : interactionHandlers) {
        if (x > ih.minx && x < ih.maxx && y > ih.miny && y < ih.maxy) {
            Serial.println("Found gesture handler");
            ih.callback(gesture, x, y);
        }
    }
}

void snFullScreenHandler(String gesture, int x, int y) {
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