#include "screens/watchface.h"
#include "screens/notificationscreen.h"
#include "touchscreen.h"
#include "time.h"
#include "declarations.h"
#include "ble.h"
#include <TFT_eSPI.h>

void wfFullScreenHandler(String gesture, int x, int y);

bool isDigitalFace = true;

// Global Variables for Analog Face
float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0; // x and y multipliers for hours, minutes, seconds
float hours_deg = 0, mins_deg = 0, secs_deg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120;
uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;
bool initial = 1;

uint16_t hhLenScale = 45, mhLenScale = 84, shLenScale = 90;
uint16_t hhWidth = 20, mhWidth = 6, shWidth = 3;

// Global Variables for Digital Face

void Watchface::init(TFT_eSprite* spr, int width, int height) {
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
    registerInteractionHandler(wfFullScreenHandler, 0, 240, 0, 240);

    // clock face
    spr->fillCircle(120, 120, 118, TFT_BLACK);
}

void Watchface::update() {
    time_t now;
    struct tm timeinfo;

    time(&now);
    setenv("TZ", "GMT", 1);
    tzset();

    localtime_r(&now, &timeinfo);
    
    if (isDigitalFace) {
        drawDigitalFace(timeinfo);
    } else {
        drawAnalogFace(timeinfo);
    }

    // spr->drawString(hms_buf, 120, 120);
    // spr->drawString(dmy_buf, 120, 140);
}

void Watchface::render() {
    this->update();
    spr->pushSprite(0, 0);
}

void Watchface::drawAnalogFace(struct tm timeinfo) {
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
}

void Watchface::drawDigitalFace(struct tm timeinfo) {
    char hm_buf[64];
    char dmy_buf[64];
    char ampm[4];
    strftime(hm_buf, sizeof(hm_buf), "%I:%M", &timeinfo);
    strftime(ampm, sizeof(ampm), "%p", &timeinfo);
    strftime(dmy_buf, sizeof(dmy_buf), "%a %d %b", &timeinfo);
    spr->fillScreen(TFT_BLACK);
    spr->unloadFont();
    spr->loadFont(Font64);
    spr->setTextColor(TFT_XON_BLUE, TFT_BLACK, true);
    spr->drawString(hm_buf, 120, 80);
    spr->unloadFont();
    spr->loadFont(Font28);
    spr->setTextColor(TFT_WHITE, TFT_BLACK, true);
    spr->drawString(ampm, 120, 130);
    spr->unloadFont();
    spr->loadFont(FontLight24);
    spr->drawString(dmy_buf, 120, 180);
}

void Watchface::registerInteractionHandler(InteractionCallback callback, int minx, int maxx, int miny, int maxy) {
    InteractionHandler ih = InteractionHandler{.callback = callback, .minx = minx, .maxx = maxx, .miny = miny, .maxy = maxy};
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void Watchface::registerInteractionHandler(InteractionHandler ih) {
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void Watchface::handleInteraction(String gesture, int x, int y) {
    Serial.printf("%s interaction with watchface at %d %d\n", gesture, x, y);
    // loop through interaction handlers and check if gesture is within their bounds
    for (auto ih : interactionHandlers) {
        if (x > ih.minx && x < ih.maxx && y > ih.miny && y < ih.maxy) {
            Serial.println("Found gesture handler");
            ih.callback(gesture, x, y);
        }
    }
}

void wfFullScreenHandler(String gesture, int x, int y) {
    Serial.println("Test handler called");
    if (gesture == "SWIPE DOWN") {
        
    }
    else if (gesture == "SWIPE UP") {

    }
    else if (gesture == "SWIPE LEFT") {
        switchScr(&ns);
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