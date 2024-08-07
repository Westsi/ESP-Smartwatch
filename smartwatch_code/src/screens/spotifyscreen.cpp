#include "screens/spotifyscreen.h"
#include "screens/homescreen.h"
#include "touchscreen.h"
#include "time.h"
#include "declarations.h"
#include "ble.h"
#include <TFT_eSPI.h>

#include "icons/pause_48.h"
#include "icons/play_48.h"
#include "icons/skipnext_24.h"
#include "icons/skipprevious_24.h"

spotify_info_t spotdata = {};

uint16_t greenplay48[48*48];
uint16_t greenpause48[48*48];

void spFullScreenHandler(String gesture, int x, int y);


void SpotifyScreen::init(TFT_eSprite* spr, int width, int height) {
    // setup
    iHLen = 0;
    this->spr = spr;
    spr->loadFont(FontLight16);
    spr->setColorDepth(8);
    spr->createSprite(width, height);
    spr->fillSprite(TFT_BLACK);
    spr->setTextColor(TFT_WHITE, TFT_BLACK, true);
    spr->setTextDatum(MC_DATUM);
    spr->setTextWrap(true);
    registerInteractionHandler(spFullScreenHandler, 0, 240, 0, 240);
    spotdata.trackname = "";
    spotdata.albumname = "";
    spotdata.artistname = "";
    spotdata.length = 0;
    spotdata.playpos = 0;
    spotdata.isplaying = false;
    recolorImage(play_48, 48, 48, TFT_SPOT_GREEN, greenplay48);
    recolorImage(pause_48, 48, 48, TFT_SPOT_GREEN, greenpause48);
}

void SpotifyScreen::update() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long long secondssinceepoch = (unsigned long long)(tv.tv_sec);
    int fnplaypos = spotdata.playpos;
    if (secondssinceepoch > spotdata.updatetime && spotdata.isplaying) {
        // Serial.printf("time delta %llu, sse %llu, updt %llu\n", (secondssinceepoch - spotdata.updatetime), secondssinceepoch, spotdata.updatetime);
        fnplaypos = fnplaypos + (secondssinceepoch - spotdata.updatetime);
        if (fnplaypos > spotdata.length) {fnplaypos = spotdata.length;}
    }

    spr->fillScreen(TFT_BLACK);

    int lengthm = spotdata.length/60;
    int lengths = spotdata.length%60;
    int ppm = fnplaypos/60;
    int pps = fnplaypos%60;
    char playtimeStr[16];
    if (pps < 10) {
        snprintf(playtimeStr, sizeof(playtimeStr), "%d:0%d/%d:%d", ppm, pps, lengthm, lengths);
    } else if (lengths < 10) {
        snprintf(playtimeStr, sizeof(playtimeStr), "%d:%d/%d:0%d", ppm, pps, lengthm, lengths);
    } else {
        snprintf(playtimeStr, sizeof(playtimeStr), "%d:%d/%d:%d", ppm, pps, lengthm, lengths);
    }

    spr->setSwapBytes(true);
    if (spotdata.isplaying) {
        spr->pushImage(120-24, 110-24, 48, 48, greenpause48);
    } else {
        spr->pushImage(120-24, 110-24, 48, 48, greenplay48);
    }
    spr->pushImage(42, 110-12, 24, 24, skipprevious_24);
    spr->pushImage(174, 110-12, 24, 24, skipnext_24);

    spr->setTextColor(TFT_XON_DARK_BLUE, TFT_BLACK, true);
    spr->drawString(spotdata.trackname.c_str(), 120, 60);

    spr->setTextColor(TFT_WHITE, TFT_BLACK, true);
    spr->loadFont(FontLight20);
    spr->drawString(spotdata.artistname.c_str(), 120, 160);

    spr->loadFont(FontLight24);
    spr->drawString(playtimeStr, 120, 200);

    spr->loadFont(FontLight16);
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

void parseSpotify(std::string text) {
    Serial.println("CALLED PARSE SPOTIFY");
    Serial.println("Spotify Text:");
    Serial.printf("\t%s\n", text.c_str());

    
    spotdata.trackname = "MISFORMATTED";
    spotdata.artistname = "MISFORMATTED";
    spotdata.albumname = "MISFORMATTED";
    spotdata.isplaying = false;
    spotdata.length = 0;
    spotdata.playpos = 0;

    // get start indices of sections
    int trackIndex = text.find("~TRACKNAME:");
    if (trackIndex == std::string::npos) {
        Serial.println("misformatted track name in spotify data");
        return;
    }

    int albumIndex = text.find("~ALBUMNAME:");
    if (albumIndex == std::string::npos) {
        Serial.println("misformatted album name in spotify data");
        return;
    }

    int artistIndex = text.find("~ARTISTNAME:");
    if (artistIndex == std::string::npos) {
        Serial.println("misformatted artist name in spotify data");
        return;
    }

    int lengthIndex = text.find("~LENGTH:");
    if (lengthIndex == std::string::npos) {
        Serial.println("misformatted length in spotify data");
        return;
    }

    int playIndex = text.find("~PLAYSTATUS:");
    if (playIndex == std::string::npos) {
        Serial.println("misformatted playstatus in spotify data");
        return;
    }

    int playPosIndex = text.find("~PLAYPOS:");
    if (playPosIndex == std::string::npos) {
        Serial.println("misformatted play position in spotify data");
        return;
    }

    int updtIndex = text.find("~UPDATETIME:");
    if (updtIndex == std::string::npos) {
        Serial.println("misformatted update time in spotify data");
        return;
    }


    // ensure that contents are arranged in format ~TRACKNAME:...~ALBUMNAME:... etc
    int order[7] = {trackIndex, albumIndex, artistIndex, lengthIndex, playIndex, playPosIndex, updtIndex};
    int sort[7] = {trackIndex, albumIndex, artistIndex, lengthIndex, playIndex, playPosIndex, updtIndex};
    int n = sizeof(sort) / sizeof(sort[0]);
    std::sort(sort, sort + n);

    for (int i=0;i<5;i++) {
        if (order[i] != sort[i]) {
            Serial.println("bad order in spotify data");
            spotdata.trackname = "BAD ORDER";
            spotdata.artistname = "BAD ORDER";
            spotdata.albumname = "BAD ORDER";
            spotdata.isplaying = false;
            spotdata.length = 0;
            return;
        }
    }
    // app name index will be appIndex+5 and length will be titleIndex-(appIndex+5)
    // same for others with different magic numbers due to different lengths of strings in find
    spotdata.trackname = text.substr(trackIndex+11, albumIndex-(trackIndex+11));
    spotdata.albumname = text.substr(albumIndex+11, artistIndex-(albumIndex+11));
    spotdata.artistname = text.substr(artistIndex+12, lengthIndex-(artistIndex+12));
    spotdata.length = atoi(text.substr(lengthIndex+8, playIndex-(lengthIndex+8)).c_str())/1000;
    spotdata.isplaying = (text.substr(playIndex+12, playPosIndex-(playIndex+12)) == "true");
    spotdata.playpos = atoi(text.substr(playPosIndex+9, updtIndex-(playPosIndex+9)).c_str())/1000;
    // spotdata.updatetime = strtoull(text.substr(updtIndex+12).c_str(), NULL, 10)/1000;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    spotdata.updatetime = (unsigned long long)(tv.tv_sec);
    Serial.println(text.substr(playIndex+12, playPosIndex-(playIndex+12)).c_str());

    Serial.printf("Parsed spotify data successfully\nTrack: %s, Album: %s, Artist: %s, Length: %ds, Playing: %d, Play position: %ds\n", 
        spotdata.trackname.c_str(), spotdata.albumname.c_str(), spotdata.artistname.c_str(), 
        spotdata.length, spotdata.isplaying, spotdata.playpos);
}