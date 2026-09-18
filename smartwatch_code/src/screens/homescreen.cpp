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
#include <math.h>

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
int scrollVel = 0; // pixels per second
float scrollDecay = 0.94f;
long startDrag;
bool isDragging = false;
const int topPadding = 20;
int maxScroll;

long lastFrameMillis;


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
    maxScroll = appTotalSpacing * (nIcons - 1);
}

/*
Continuous Scrolling function
 */
// void Homescreen::update() {
//     spr->fillScreen(TFT_BLACK);
//     spr->setSwapBytes(true);
//     int renderYTop = scrollY;
//     int renderYBase = scrollY + 240;
//     int cumY = topPadding;
//     for (int i=0;i<nIcons;i++) {
//         app_icon_t* ai = appIcons[i];
//         int aiyTop = cumY;
//         int aiyBase = cumY + appTotalSpacing;
//         cumY += appTotalSpacing;
//         if (aiyTop > renderYBase) {
//             // passed the screen range, anything past here is definitely out of range
//             break;
//         }
//         if (aiyBase < renderYTop) {
//             // not at screen range yet, increment cumY and continue
//             continue;
//         }
//         aiyTop -= scrollY;
//         aiyBase -= scrollY;
//         if (aiyTop > aiyBase) {
//             // something has gone wrong
//             spr->fillScreen(TFT_RED);
//         }
//         spr->pushImage(40, aiyTop, iconWidth, iconWidth, ai->icon);
//         spr->drawString(ai->scrname, 120, aiyTop + iconWidth + iconNameSpacing);
//     }
    
// }


// hex grid
void Homescreen::update() {
    spr->fillScreen(TFT_BLACK);
    spr->setSwapBytes(true);
    // handle continued scrolling
    // decay
    scrollVel *= scrollDecay;
    if (abs(scrollVel) <= 2) scrollVel = 0;
    if (scrollVel != 0) {
        int elapsedMillis = millis() - lastFrameMillis;
        float dScroll = ((float) scrollVel * (float) elapsedMillis) / 1000.0f;
        scrollY += dScroll;
        scrollY = min(scrollY, maxScroll);
        scrollY = max(0, scrollY);
        Serial.printf("ScrollVel = %d, ScrollY = %d, dScroll = %f\n", scrollVel, scrollY, dScroll);
    }

    lastFrameMillis = millis();

    // initially we just draw coloured circles to test it
    const int lPad = 60;
    const int tPad = 100;
    const int baseSpacingX = 60; // horiz dist b/w cols
    const int baseSpacingY = 52; //. for perfect hexagon spacingY = spacingX * sqrt(3)/2
    const int baseRadius = 30;
    const int centerX = 120;
    const int centerY = 120;
    int colors[] = {TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_BLUE, TFT_MAGENTA, TFT_VIOLET};
    int ncols = 7;

    // TODO: make this do icons instead of circles
    // TODO: remove row 0
    int cnt = 100;
    for (int i=0;i<cnt;i++) {
        // we need to dynamically find the gridX and Y coordinates - we can fit 2 icons per odd row, 3 per even row
        // this needs to map to 0,0 0,1 0,2
        //                      1,0, 1,1
        // we can do this by finding quotient when divided by 5 to get multiples of two rows - EOEO...
        // counting for these starts from top left
        int gridY = 2 * ((int)(i / 5));
        int gridX = i % 5;
        if (gridX >= 3) {
            gridX -= 3;
            gridY += 1;
        }

        // now find x and y scr positions for a hex grid and offset odd rows by 1/2 col width
        int posX = gridX * baseSpacingX + lPad;
        int posY = gridY * baseSpacingY - scrollY + tPad;

        if (gridY % 2 != 0) {
            posX += baseSpacingX / 2;
        }

        // find distance from screen centre
        int dx = posX - centerX;
        int dy = posY - centerY;
        float dSquared = (dx*dx) + (dy*dy);
        float maxDSquared = 120 * 120;

        // fisheye scaling
        float rscale = 1.0f - (dSquared / maxDSquared);
        rscale = min(max(rscale, 0.4f), 1.0f); // clamp min and max between 0.4 and 1

        float dscale = 1.0f - 0.7f * (dSquared / maxDSquared);
        dscale = min(max(dscale, 0.3f), 1.0f);

        // compress icons towards centre
        int finalX = centerX + (int)(dx * dscale);
        int finalY = centerY + (int)(dy * dscale);
        int currentRadius = (int)(baseRadius * rscale);

        // calculate color
        int color = colors[i % ncols];

        // render circle
        spr->fillCircle(posX, posY, currentRadius, color);
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
    // Serial.printf("%s interaction with homescreen at %d %d\n", gesture, x, y);
    // loop through interaction handlers and check if gesture is within their bounds
    for (auto ih : interactionHandlers) {
        if (x > ih.minx && x < ih.maxx && y > ih.miny && y < ih.maxy) {
            // Serial.println("Found gesture handler");
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
    if (touchEvent == 0 || (touchEvent == 2 && !isDragging)) { // touch down
        scrollVel = 0;
        startTouchY = touch.data.y;
        baseScrollY = scrollY;
        startDrag = millis();
        isDragging = true;
    } else if (touchEvent == 2 && isDragging) { // touch move
        int deltaY = touch.data.y - startTouchY;
        // update
        scrollY = baseScrollY - deltaY;
        // clamp
        if (scrollY < 0) scrollY = 0;
        if (scrollY > maxScroll) scrollY = maxScroll;
    } else if (touchEvent == 1) { // touch up
        // calculate velocity
        int totalDelta = startTouchY - touch.data.y;
        float elapsedSec = ((float) millis() - (float) startDrag) / 1000.0f;
        scrollVel = totalDelta / elapsedSec;
        Serial.printf("SCROLLVEL = %d, TOTALDELTA = %d, ELAPSEDSEC = %f\n", scrollVel, totalDelta, elapsedSec);
        isDragging = false;
    }

    
    // Serial.println("Test handler called");
    if (gesture == "SINGLE CLICK") {
        // switchScr(appIcons[activeicon]->scr);
        // animateSwitch(ANIMATE_IN_FROM_RIGHT, &hs, appIcons[activeicon]->scr);
        if (scrollVel != 0) {
            scrollVel = 0;
            return;
        }
        Screen* clickedScreen = getIconFromCoords(x, y);
        if (clickedScreen != NULL) sweepAnimation(ANIMATE_IN_FROM_RIGHT, &hs, clickedScreen);
    }
}

Screen* getIconFromCoords(int x, int y) {
    // for now, only y really matters
    int realY = y + scrollY;
    int cumY = topPadding;
    for (int i=0;i<nIcons;i++) {
        app_icon_t* ai = appIcons[i];
        int aiyTop = cumY;
        int aiyBase = cumY + appTotalSpacing;
        Serial.printf("App %s aiyTop=%d, aiyBase=%d, realY=%d\n", ai->scrname, aiyTop, aiyBase, realY);
        if (realY > aiyBase) {
            cumY += appTotalSpacing;
            Serial.printf("Incremented cumY=%d, realY=%d\n", cumY, realY);
            continue;
        }
        if (realY >= aiyTop && realY <= aiyBase) {
            // hit!
            Serial.printf("Hit on %s\n", ai->scrname);
            return ai->scr;
        }
    }
    Serial.println("No screens matched!");
    return NULL;
}