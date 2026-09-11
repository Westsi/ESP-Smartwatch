#include "screens/homescreen.h"
#include "screens/watchface.h"
#include "screens/notificationscreen.h"
#include "screens/exercisescreen.h"
#include "screens/settingsscreen.h"
#include "screens/spotifyscreen.h"
#include "screens/blackjackscreen.h"
#include "screens/stopwatch.h"
#include "touchscreen.h"
#include "time.h"
#include "declarations.h"
#include "ble.h"
#include <TFT_eSPI.h>

#include "icons/schedule_160.h" // watchface, stopwatch
#include "icons/spotify_160.h" // spotify
#include "icons/steps_160.h" // exercise tracker
#include "icons/settings_160.h" // settings
#include "icons/notifications_160.h" // notifications
#include "icons/play_160.h" // blackjack

void hsFullScreenHandler(String gesture, int x, int y);
Screen* getIconFromCoords(int x, int y);

typedef struct hsaicon {
    const prog_uint16_t* icon;
    Screen* scr;
    const char* scrname;
} app_icon_t;

app_icon_t watchfaceicon = {.icon=schedule_160, .scr=&wf, .scrname="Watch"};
app_icon_t stopwatchicon = {.icon=schedule_160, .scr=&sw, .scrname="Stopwatch"};
app_icon_t notificationsicon = {.icon=notifications_160, .scr=&ns, .scrname="Notifications"};
app_icon_t spotifyicon = {.icon=spotify_160, .scr=&sp, .scrname="Spotify"};
app_icon_t blackjackicon = {.icon=play_160, .scr=&bs, .scrname="Blackjack"};
app_icon_t exerciseicon = {.icon=steps_160, .scr=&es, .scrname="Exercise"};
app_icon_t settingsicon = {.icon=settings_160, .scr=&ss, .scrname="Settings"};

app_icon_t* appIcons[] = {&watchfaceicon, &stopwatchicon, &notificationsicon, &spotifyicon, &blackjackicon, &exerciseicon, &settingsicon};
int nIcons = 7;

// int activeicon = 0;

uint8_t iconNameSpacing = 20; // spacing between bottom of icon and mid of text
uint8_t appTotalSpacing = 200;
uint8_t iconWidth = 160; // also iconHeight

// proper scrolling
int scrollY = 0;
int baseScrollY = 0;
int startTouchY = 0;
bool isDragging = false;
const int maxScroll = 400;


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
    // app_icon_t* ic = appIcons[activeicon];
    // spr->pushImage(40, 20, 160, 160, ic->icon);
    // spr->drawString(appIcons[activeicon]->scrname, 120, 200);
    int renderYTop = scrollY;
    int renderYBase = scrollY + 240;
    int cumY = 20;
    for (int i=0;i<nIcons;i++) {
        app_icon_t* ai = appIcons[i];
        int aiyTop = cumY;
        int aiyBase = cumY + appTotalSpacing;
        if (aiyTop > renderYBase) {
            // passed the screen range, anything past here is definitely out of range
            break;
        }
        if (aiyBase < renderYTop) {
            // not at screen range yet, increment cumY and continue
            cumY += appTotalSpacing;
            continue;
        }
        aiyTop -= scrollY;
        aiyBase -= scrollY;
        if (aiyTop > aiyBase || aiyTop < 0 || aiyBase > 240) {
            // something has gone wrong
            spr->fillScreen(TFT_RED);
        }
        spr->pushImage(40, aiyTop, iconWidth, iconWidth, ai->icon);
        spr->drawString(ai->scrname, 120, aiyTop + iconWidth + iconNameSpacing);
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
    // int scrollY = 0;
    // int baseScrollY = 0;
    // int startTouchY = 0;
    // bool isDragging = false;
    // const int maxScroll = 400;

    uint8_t touchEvent = touch.data.event;
    if (touchEvent == 0) { // touch down
        startTouchY = touch.data.y;
        baseScrollY = scrollY;
        isDragging = true;
    } else if (touchEvent == 2 && isDragging) { // touch move
        int deltaY = touch.data.y - startTouchY;
        // update
        scrollY = baseScrollY - deltaY;
        // clamp
        if (scrollY < 0) scrollY = 0;
        if (scrollY > maxScroll) scrollY = maxScroll;
    } else if (touchEvent == 1) { // touch up
        isDragging = false;
    }

    
    Serial.println("Test handler called");
    if (gesture == "SINGLE CLICK") {
        // switchScr(appIcons[activeicon]->scr);
        // animateSwitch(ANIMATE_IN_FROM_RIGHT, &hs, appIcons[activeicon]->scr);
        Screen* clickedScreen = getIconFromCoords(x, y);
        sweepAnimation(ANIMATE_IN_FROM_RIGHT, &hs, clickedScreen);
    }
}

Screen* getIconFromCoords(int x, int y) {
    // for now, only y really matters
    int realY = y + scrollY;
    int cumY = 20;
    for (int i=0;i<nIcons;i++) {
        app_icon_t* ai = appIcons[i];
        int aiyTop = cumY;
        int aiyBase = cumY + appTotalSpacing;
        if (realY < aiyTop) {
            cumY += appTotalSpacing;
            continue;
        }
        if (realY >= aiyTop && realY <= aiyBase) {
            // hit!
            return ai->scr;
        }
    }
}