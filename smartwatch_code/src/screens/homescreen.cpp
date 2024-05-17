#include "screens/homescreen.h"
#include "touchscreen.h"
#include "time.h"
#include "declarations.h"
#include "ble.h"
#include <TFT_eSPI.h>

void testHandler(String gesture, int x, int y);

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0; // x and y multipliers for hours, minutes, seconds
float hours_deg = 0, mins_deg = 0, secs_deg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120;
uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;
bool initial = 1;

uint16_t hhLenScale = 45, mhLenScale = 84, shLenScale = 90;
uint16_t hhWidth = 20, mhWidth = 6, shWidth = 3;

void Homescreen::init(TFT_eSprite* spr, int width, int height) {
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
    registerInteractionHandler(testHandler, 0, 240, 0, 240);

    // clock face
    spr->fillCircle(120, 120, 118, TFT_BLACK);
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

    int hours = timeinfo.tm_hour;
    int mins = timeinfo.tm_min;
    int secs = timeinfo.tm_sec;

    secs_deg = secs*6;                  // 0-59 -> 0-354
    mins_deg = mins*6+secs_deg*0.01666667;  // 0-59 -> 0-360 - includes seconds
    hours_deg = hours*30+mins_deg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
    hx = cos((hours_deg-90)*0.0174532925);    
    hy = sin((hours_deg-90)*0.0174532925);
    mx = cos((mins_deg-90)*0.0174532925);    
    my = sin((mins_deg-90)*0.0174532925);
    sx = cos((secs_deg-90)*0.0174532925);    
    sy = sin((secs_deg-90)*0.0174532925);

    if (secs == 0 || initial) {
        initial = 0;
        // Erase hour and minute hand positions every minute
        spr->drawWideLine(ohx, ohy, 120, 121, hhWidth+2, TFT_BLACK);
        ohx = hx*hhLenScale+121;
        ohy = hy*hhLenScale+121;
        spr->drawWideLine(omx, omy, 120, 121, mhWidth+2, TFT_BLACK);
        omx = mx*mhLenScale+120;    
        omy = my*mhLenScale+121;
    }

    // Redraw new hand positions, hour and minute hands not erased here to avoid flicker

    spr->drawWideLine(osx, osy, 120, 121, shWidth+2, TFT_BLACK);
    osx = sx*shLenScale+121;    
    osy = sy*shLenScale+121;
    spr->drawWideLine(osx, osy, 120, 121, shWidth, TFT_XON_DARK_BLUE);
    spr->drawWideLine(ohx, ohy, 120, 121, hhWidth, TFT_XON_BLUE);
    spr->drawWideLine(omx, omy, 120, 121, mhWidth, TFT_WHITE);
    spr->drawWideLine(osx, osy, 120, 121, shWidth, TFT_XON_DARK_BLUE);

    // spr->drawString(hms_buf, 120, 120);
    // spr->drawString(dmy_buf, 120, 140);
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
        Serial.println("Updating notifs");
        sendCommand("UPDATE_NOTIFS");
    }
    else if (gesture == "DOUBLE CLICK") {

    }
    else if (gesture == "LONG PRESS") {

    }
}