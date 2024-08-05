#include "screens/notificationscreen.h"
#include "screens/watchface.h"
#include "screens/homescreen.h"
#include "touchscreen.h"
#include "time.h"
#include "declarations.h"
#include "ble.h"
#include <TFT_eSPI.h>
#include "notifications.h"

void closeNotificationHandler(String gesture, int x, int y);
void nsFullScreenHandler(String gesture, int x, int y);

void NotificationScreen::init(TFT_eSprite* spr, int width, int height) {
    // setup
    iHLen = 0;
    this->spr = spr;
    spr->loadFont(FontLight20);
    spr->setColorDepth(8);
    spr->createSprite(width, height);
    spr->fillSprite(TFT_BLACK);
    spr->setTextColor(TFT_WHITE, TFT_BLACK, true);
    spr->setTextDatum(MC_DATUM);
    spr->setTextWrap(true, true);
    registerInteractionHandler(closeNotificationHandler, 130, 170, 15, 45);
    registerInteractionHandler(nsFullScreenHandler, 0, 240, 0, 240);
}

void NotificationScreen::update() {
    spr->fillSprite(TFT_BLACK);
    // spr->drawString("HI", 120, 120);
    Notification* notif = getNotification(0);
    if (notif != NULL) {
        spr->drawString(String(notif->app.c_str()), 120, 60);
        spr->drawString(String(notif->title.c_str()), 120, 90);
        spr->unloadFont();
        spr->loadFont(FontLight18);
        spr->drawString(String(notif->contents.c_str()), 120, 110);
        spr->unloadFont();
        spr->loadFont(FontLight28);
        spr->fillSmoothRoundRect(130, 15, 40, 30, 10, TFT_XON_BLUE);
        spr->setTextColor(TFT_RED, TFT_XON_BLUE, true);
        spr->drawString("X", 150, 30);
        spr->unloadFont();
        spr->loadFont(FontLight20);
        spr->setTextColor(TFT_WHITE, TFT_BLACK, true);
    } else {
        spr->drawString("No notifications pending.", 120, 120);
    }
}

void NotificationScreen::render() {
    this->update();
    spr->pushSprite(0, 0);
}

void NotificationScreen::registerInteractionHandler(InteractionCallback callback, int minx, int maxx, int miny, int maxy) {
    InteractionHandler ih = InteractionHandler{.callback = callback, .minx = minx, .maxx = maxx, .miny = miny, .maxy = maxy};
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void NotificationScreen::registerInteractionHandler(InteractionHandler ih) {
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void NotificationScreen::handleInteraction(String gesture, int x, int y) {
    Serial.printf("%s interaction with notification screen at %d %d\n", gesture, x, y);
    // loop through interaction handlers and check if gesture is within their bounds
    for (auto ih : interactionHandlers) {
        if (ih.maxx == 0 && ih.maxy == 0) {
            continue;
        }
        Serial.printf("minx %d maxx %d miny %d maxy %d\n", ih.minx, ih.maxx, ih.miny, ih.maxy);
        if (x > ih.minx && x < ih.maxx && y > ih.miny && y < ih.maxy) {
            Serial.println("Found gesture handler");
            ih.callback(gesture, x, y);
            break;
        }
    }
}

void closeNotificationHandler(String gesture, int x, int y) {
    Serial.println("Closing notification");
    if (gesture == "SINGLE CLICK") {
        deleteNotification(0);
    }
}

void nsFullScreenHandler(String gesture, int x, int y) {
    Serial.println("Test handler called");
    if (gesture == "SWIPE RIGHT") {
        switchScr(&hs);
    }
}