#include "screens/homescreen.h"
#include "touchscreen.h"
#include "declarations.h"
#include <TFT_eSPI.h>

void Homescreen::init(TFT_eSprite* spr, int width, int height) {
    iHLen = 0;
    this->spr = spr;
    spr->loadFont(FontLight14);
    spr->setColorDepth(8);
    spr->createSprite(width, height);
    spr->fillSprite(TFT_BLUE);
    spr->setTextColor(TFT_BLACK, TFT_BLUE, true);
    spr->setTextDatum(MC_DATUM);
    spr->setTextWrap(true);
    spr->drawString("Xonize Smartwatch", 120, 120);
}

void Homescreen::update() {
    Serial.println(millis());
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
}