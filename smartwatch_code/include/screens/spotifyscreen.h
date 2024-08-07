#ifndef SPOTIFYSCREEN_H_
#define SPOTIFYSCREEN_H_

#include "touchscreen.h"
#include <TFT_eSPI.h>

class SpotifyScreen : public Screen {
    public:
        void init(TFT_eSprite* spr, int width, int height);
        void update();
        void render();
        void registerInteractionHandler(InteractionCallback callback, int minx, int maxx, int miny, int maxy);
        void registerInteractionHandler(InteractionHandler ih);
        void handleInteraction(String gesture, int x, int y);
};

extern SpotifyScreen sp;

typedef struct SPOTINF {
    std::string trackname;
    std::string albumname;
    std::string artistname;
    bool isplaying;
    int length;
    int playpos;
    unsigned long long updatetime;
} spotify_info_t;

extern spotify_info_t spotdata;

void parseSpotify(std::string text);


#endif