#include "screens/exercisescreen.h"
#include "stepcounter.h"
#include "touchscreen.h"
#include "declarations.h"
#include "screens/watchface.h"
#include "ble.h"
#include <TFT_eSPI.h>

void esFullScreenHandler(String gesture, int x, int y);


void ExerciseScreen::init(TFT_eSprite* spr, int width, int height) {
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
    registerInteractionHandler(esFullScreenHandler, 0, 240, 0, 240);

    // clock face
    spr->fillCircle(120, 120, 118, TFT_BLACK);
}

void ExerciseScreen::update() {
    long angle = map(getSteps(), 0, 100, 180, 540);
    // spr->drawSmoothArc(120, 120, 110, 100, 90, angle, TFT_XON_BLUE, TFT_BLACK, true);
    // x and y are centers, r is outer radius, ir is inner radius
    // arcs should be coloured, starting from north and going clockwise
    // they should also be drawn on circles that are less grey than the background
    spr->drawSmoothArc(120, 120, 50, 40, 0, 360, TFT_DARKGREY, TFT_BLACK, true); // draw grey background circle
    if (angle >=540) { // full circle
        spr->drawSmoothArc(120, 120, 50, 40, 0, 360, TFT_XON_BLUE, TFT_DARKGREY, false);
    }
    spr->drawSmoothArc(120, 120, 50, 40, 180, angle%360, TFT_XON_BLUE, TFT_DARKGREY, true);
}

void ExerciseScreen::render() {
    this->update();
    spr->pushSprite(0, 0);
}

void ExerciseScreen::registerInteractionHandler(InteractionCallback callback, int minx, int maxx, int miny, int maxy) {
    InteractionHandler ih = InteractionHandler{.callback = callback, .minx = minx, .maxx = maxx, .miny = miny, .maxy = maxy};
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void ExerciseScreen::registerInteractionHandler(InteractionHandler ih) {
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void ExerciseScreen::handleInteraction(String gesture, int x, int y) {
    Serial.printf("%s interaction with watchface at %d %d\n", gesture, x, y);
    // loop through interaction handlers and check if gesture is within their bounds
    for (auto ih : interactionHandlers) {
        if (x > ih.minx && x < ih.maxx && y > ih.miny && y < ih.maxy) {
            Serial.println("Found gesture handler");
            ih.callback(gesture, x, y);
        }
    }
}

void esFullScreenHandler(String gesture, int x, int y) {
    Serial.println("Test handler called");
    if (gesture == "SWIPE DOWN") {
        
    }
    else if (gesture == "SWIPE UP") {
        switchScr(&wf);
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