#ifndef BLACKJACKSCREEN_H_
#define BLACKJACKSCREEN_H_

#include "touchscreen.h"
#include <TFT_eSPI.h>

class BlackjackScreen : public Screen {
    public:
        void init(TFT_eSprite* spr, int width, int height);
        void update();
        void render();
        void registerInteractionHandler(InteractionCallback callback, int minx, int maxx, int miny, int maxy);
        void registerInteractionHandler(InteractionHandler ih);
        void handleInteraction(String gesture, int x, int y);
};

extern BlackjackScreen bs;


#endif