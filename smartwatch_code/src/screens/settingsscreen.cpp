#include "screens/settingsscreen.h"
#include "hardware_interface.h"
#include "touchscreen.h"
#include "screens/homescreen.h"
#include "screens/watchface.h"
#include "time.h"
#include "declarations.h"
#include "ble.h"
#include <TFT_eSPI.h>
#include <math.h>
#include "config.h"

void ssFullScreenHandler(String gesture, int x, int y);
void sliderHandler(String gesture, int x, int y);
void toggleHandler(String gesture, int x, int y);


int knobVal = 50; // 1-255

void SettingsScreen::init(TFT_eSprite* spr, int width, int height) {
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
    registerInteractionHandler(sliderHandler, 50, 207, 60, 120); // make sure this is put in first so it is called first
    registerInteractionHandler(toggleHandler, 60, 180, 130, 190);
    registerInteractionHandler(ssFullScreenHandler, 0, 240, 0, 240);

    knobVal = log((double)getScreenBrightness()) / 0.02173;
}



void SettingsScreen::update() {
    spr->drawString("Settings", 120, 40);
    // draw slider
    spr->fillSmoothRoundRect(60, 80, 120, 40, 20, TFT_DARKGREY, TFT_BLACK);
    // knob
    // calculate relative pos of slider
    int relx = (int)(86.0 * (float)((float)knobVal/255));
    spr->fillSmoothCircle(77 + relx, 100, 17, TFT_XON_BLUE, TFT_DARKGREY);

    char batt[8];
    uint8_t b = getBatteryPercentage();
    snprintf(batt, sizeof(batt), "%d%%", b);
    spr->drawString(batt, 120, 200);

    
    uint32_t buttonCol = (isDigitalFace) ? TFT_XON_BLUE : TFT_LIGHTGREY;
    spr->fillSmoothRoundRect(60, 140, 120, 40, 10, buttonCol);
    spr->setTextColor(TFT_WHITE, buttonCol, true);
    spr->drawString("Digital Face?", 120, 160);
    spr->setTextColor(TFT_WHITE, TFT_BLACK, true);
    
}

void SettingsScreen::render() {
    this->update();
    spr->pushSprite(0, 0);
}

void SettingsScreen::registerInteractionHandler(InteractionCallback callback, int minx, int maxx, int miny, int maxy) {
    InteractionHandler ih = InteractionHandler{.callback = callback, .minx = minx, .maxx = maxx, .miny = miny, .maxy = maxy};
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void SettingsScreen::registerInteractionHandler(InteractionHandler ih) {
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void SettingsScreen::handleInteraction(String gesture, int x, int y) {
    Serial.printf("%s interaction with settings screen at %d %d\n", gesture, x, y);
    // loop through interaction handlers and check if gesture is within their bounds
    for (auto ih : interactionHandlers) {
        if (x > ih.minx && x < ih.maxx && y > ih.miny && y < ih.maxy) {
            Serial.println("Found gesture handler");
            ih.callback(gesture, x, y);
            break;
        }
    }
}

void toggleHandler(String gesture, int x, int y) {
    if (gesture == "SINGLE CLICK") {
        isDigitalFace = !isDigitalFace;
        saveWatchFace(isDigitalFace);
    } else {
        ssFullScreenHandler(gesture, x, y);
    }
}

void sliderHandler(String gesture, int x, int y) {
    // rearrangement of formula to make knobVal -> absolute xpos from above
    int calcKV = ((x-77)*255)/86;
    calcKV = min(max(calcKV, 0), 255);
    knobVal = calcKV;
    setScreenBrightness((int)exp(0.02173 * (double)calcKV));
}

void ssFullScreenHandler(String gesture, int x, int y) {
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
        
    }
    else if (gesture == "DOUBLE CLICK") {

    }
    else if (gesture == "LONG PRESS") {

    }
}