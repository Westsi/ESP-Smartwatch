#include "config.h"
#include <Preferences.h>

Preferences prefs;

void initConfig() {
    prefs.begin("xsw", false); // false = r/w mode
}

int loadBrightness() {
    if (!prefs.isKey(BRIGHTNESS_KEY)) {
        Serial.println("saving brightness");
        saveBrightness(255);
    }
    int b = prefs.getInt(BRIGHTNESS_KEY, 255); // set 255 to default in case
    Serial.printf("%d\n", b);
    return b;
}

void saveBrightness(int brightness) {
    prefs.putInt(BRIGHTNESS_KEY, brightness);
}

void saveWatchFace(int wf) {
    prefs.putInt(WATCHFACE_KEY, wf);
}

int loadWatchFace() {
    if (!prefs.isKey(WATCHFACE_KEY)) {
        Serial.println("saving watchface");
        saveWatchFace(WATCHFACE_DIGITAL);
    }
    int b = prefs.getInt(WATCHFACE_KEY, WATCHFACE_DIGITAL); // set 255 to default in case
    Serial.printf("%d\n", b);
    return b;
}