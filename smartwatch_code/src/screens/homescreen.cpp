#include "screens/homescreen.h"
#include "screens/watchface.h"
#include "screens/notificationscreen.h"
#include "screens/exercisescreen.h"
#include "screens/settingsscreen.h"
#include "screens/spotifyscreen.h"
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
    const prog_uint16_t* icon;
    Screen* scr;
    const char* scrname;
} app_icon_t;

app_icon_t watchfaceicon = {.icon=schedule_160, .scr=&wf, .scrname="Watch"};
app_icon_t spotifyicon = {.icon=spotify_160, .scr=&sp, .scrname="Spotify"};
app_icon_t exerciseicon = {.icon=steps_160, .scr=&es, .scrname="Exercise"};
app_icon_t settingsicon = {.icon=settings_160, .scr=&ss, .scrname="Settings"};
app_icon_t notificationsicon = {.icon=notifications_160, .scr=&ns, .scrname="Notifications"};

app_icon_t* appIcons[] = {&watchfaceicon, &notificationsicon, &spotifyicon, &exerciseicon, &settingsicon};

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
    spr->fillScreen(TFT_BLACK);
    spr->setSwapBytes(true);
    app_icon_t* ic = appIcons[activeicon];
    spr->pushImage(40, 20, 160, 160, ic->icon);
    spr->drawString(appIcons[activeicon]->scrname, 120, 200);
    if (activeicon != 0) {
        spr->fillTriangle(120, 5, 110, 15, 130, 15, TFT_XON_BLUE);
    }
    if (activeicon != (sizeof(appIcons) / sizeof(appIcons[0])) - 1) {
        spr->fillTriangle(120, 235, 110, 225, 130, 225, TFT_XON_BLUE);
    }
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
        // switchScr(appIcons[activeicon]->scr);
        animateSwitch(ANIMATE_IN_FROM_RIGHT, &hs, appIcons[activeicon]->scr);
    }
    else if (gesture == "SWIPE RIGHT") {

    }
    else if (gesture == "SINGLE CLICK") {
        // switchScr(appIcons[activeicon]->scr);
        animateSwitch(ANIMATE_IN_FROM_RIGHT, &hs, appIcons[activeicon]->scr);
    }
    else if (gesture == "DOUBLE CLICK") {

    }
    else if (gesture == "LONG PRESS") {

    }
}