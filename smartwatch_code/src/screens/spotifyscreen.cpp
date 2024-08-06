#include "screens/spotifyscreen.h"
#include "screens/homescreen.h"
#include "touchscreen.h"
#include "time.h"
#include "declarations.h"
#include "ble.h"
#include <TFT_eSPI.h>

spotify_info_t* spotdata = {};

void spFullScreenHandler(String gesture, int x, int y);


void SpotifyScreen::init(TFT_eSprite* spr, int width, int height) {
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
    registerInteractionHandler(spFullScreenHandler, 0, 240, 0, 240);
}

void SpotifyScreen::update() {
    spr->drawString("Spotify!", 120, 120);
}

void SpotifyScreen::render() {
    this->update();
    spr->pushSprite(0, 0);
}

void SpotifyScreen::registerInteractionHandler(InteractionCallback callback, int minx, int maxx, int miny, int maxy) {
    InteractionHandler ih = InteractionHandler{.callback = callback, .minx = minx, .maxx = maxx, .miny = miny, .maxy = maxy};
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void SpotifyScreen::registerInteractionHandler(InteractionHandler ih) {
    this->interactionHandlers[iHLen] = ih;
    iHLen++;
}

void SpotifyScreen::handleInteraction(String gesture, int x, int y) {
    Serial.printf("%s interaction with spotify screen at %d %d\n", gesture, x, y);
    // loop through interaction handlers and check if gesture is within their bounds
    for (auto ih : interactionHandlers) {
        if (x > ih.minx && x < ih.maxx && y > ih.miny && y < ih.maxy) {
            Serial.println("Found gesture handler");
            ih.callback(gesture, x, y);
            break;
        }
    }
}

void spFullScreenHandler(String gesture, int x, int y) {
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

spotify_info_t* parseSpotify(std::string text) {
    Serial.println("CALLED PARSE NOTIFICATION");
    spotify_info_t* s;
    Serial.println("Notification Text:");
    Serial.printf("\t%s\n", text.c_str());
    // get start indices of sections
    int trackIndex = text.find("~TRACKNAME:");
    if (trackIndex == std::string::npos) {
        Serial.println("misformatted track name in spotify data");
        s->trackname = "MISFORMATTED";
        s->artistname = "MISFORMATTED";
        s->albumname = "MISFORMATTED";
        s->isplaying = false;
        s->length = 0;
        return s;
    }

    int albumIndex = text.find("~ALBUMNAME:");
    if (albumIndex == std::string::npos) {
        Serial.println("misformatted album name in spotify data");
        s->trackname = "MISFORMATTED";
        s->artistname = "MISFORMATTED";
        s->albumname = "MISFORMATTED";
        s->isplaying = false;
        s->length = 0;
        return s;
    }

    int artistIndex = text.find("~ARTISTNAME:");
    if (artistIndex == std::string::npos) {
        Serial.println("misformatted artist name in spotify data");
        s->trackname = "MISFORMATTED";
        s->artistname = "MISFORMATTED";
        s->albumname = "MISFORMATTED";
        s->isplaying = false;
        s->length = 0;
        return s;
    }

    int lengthIndex = text.find("~LENGTH:");
    if (lengthIndex == std::string::npos) {
        Serial.println("misformatted length in spotify data");
        s->trackname = "MISFORMATTED";
        s->artistname = "MISFORMATTED";
        s->albumname = "MISFORMATTED";
        s->isplaying = false;
        s->length = 0;
        return s;
    }

    int playIndex = text.find("~PLAYSTATUS:");
    if (playIndex == std::string::npos) {
        Serial.println("misformatted playstatus in spotify data");
        s->trackname = "MISFORMATTED";
        s->artistname = "MISFORMATTED";
        s->albumname = "MISFORMATTED";
        s->isplaying = false;
        s->length = 0;
        return s;
    }


    // ensure that contents are arranged in format ~TRACKNAME:...~ALBUMNAME:...~ARTISTNAME:...~LENGTH:...~PLAYSTATUS:...
    int order[5] = {trackIndex, albumIndex, artistIndex, lengthIndex, playIndex};
    int sort[5] = {trackIndex, albumIndex, artistIndex, lengthIndex, playIndex};
    int n = sizeof(sort) / sizeof(sort[0]);
    std::sort(sort, sort + n);

    for (int i=0;i<5;i++) {
        if (order[i] != sort[i]) {
            Serial.println("bad order in spotify data");
            s->trackname = "BAD ORDER";
            s->artistname = "BAD ORDER";
            s->albumname = "BAD ORDER";
            s->isplaying = false;
            s->length = 0;
            return s;
            return s;
        }
    }
    // app name index will be appIndex+5 and length will be titleIndex-(appIndex+5)
    // same for others with different magic numbers due to different lengths of strings in find
    s->trackname = text.substr(trackIndex+11, albumIndex-(trackIndex+11)).c_str();
    s->albumname = text.substr(albumIndex+11, artistIndex-(albumIndex+11)).c_str();
    s->artistname = text.substr(artistIndex+12, lengthIndex-(artistIndex+12)).c_str();
    s->length = atoi(text.substr(lengthIndex+8, playIndex-(lengthIndex+8)).c_str())/1000;
    s->isplaying = (text.substr(playIndex+12, playIndex-(lengthIndex+8)).c_str() == "true"); // runs to end of string
    Serial.println(text.substr(playIndex+12, playIndex-(lengthIndex+8)).c_str());

    Serial.printf("Parsed spotify data successfully\nTrack: %s, Album: %s, Artist: %s, Length: %ds, Playing: %d\n", s->trackname, s->albumname, s->artistname, s->length, s->isplaying);
    return s;
}