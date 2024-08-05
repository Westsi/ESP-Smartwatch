#include "screens/homescreen.h"
#include "screens/watchface.h"
#include "screens/notificationscreen.h"
#include "screens/exercisescreen.h"
#include "screens/settingsscreen.h"
#include "touchscreen.h"
#include "time.h"
#include "declarations.h"
#include "ble.h"
#include <TFT_eSPI.h>

#include "icons/schedule_160.h" // watchface
#include "icons/spotify_160.h" // spotify
#include "icons/steps_160.h" // exercise tracker
#include "icons/settings_160.h" // settings
#include "icons/notifications_160.h" // notifications

void hsFullScreenHandler(String gesture, int x, int y);

typedef struct hsaicon {
    prog_uint16_t* icon;
    Screen* scr;
} app_icon_t;

app_icon_t watchfaceicon = {.icon=schedule_160, .scr=&wf};
// app_icon_t spotifyicon = {.icon=spotify_160, .scr=&sp};
app_icon_t exerciseicon = {.icon=steps_160, .scr=&es};
app_icon_t settingsicon = {.icon=settings_160, .scr=&ss};
app_icon_t notificationsicon = {.icon=notifications_160, .scr=&ns};

app_icon_t* appIcons[] = {&watchfaceicon, &notificationsicon, &exerciseicon, &settingsicon};

int activeicon = 0;


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
    registerInteractionHandler(hsFullScreenHandler, 0, 240, 0, 240);
}

void Homescreen::update() {
    spr->drawString("Hello world!", 120, 120);
    spr->setSwapBytes(true);
    app_icon_t* ic = appIcons[activeicon];
    spr->pushImage(40, 40, 160, 160, ic->icon);
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
            break;
        }
    }
}

void hsFullScreenHandler(String gesture, int x, int y) {
    Serial.println("Test handler called");
    if (gesture == "SWIPE DOWN") {
        if (activeicon > 0) {
            activeicon--;
        }
    }
    else if (gesture == "SWIPE UP") {
        if (activeicon < (sizeof(appIcons) / sizeof(appIcons[0])) - 1) {
            activeicon++;
        }
    }
    else if (gesture == "SWIPE LEFT") {

    }
    else if (gesture == "SWIPE RIGHT") {

    }
    else if (gesture == "SINGLE CLICK") {
        // TODO: check which icon it is on and navigate to that screen
    }
    else if (gesture == "DOUBLE CLICK") {

    }
    else if (gesture == "LONG PRESS") {

    }
}