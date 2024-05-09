#ifndef TOUCH_SCREEN_H_
#define TOUCH_SCREEN_H_

#include <CST816S.h>
#include <TFT_eSPI.h>

#include "fonts/NotoSansBold36.h"
#define AA_FONT_LARGE NotoSansBold36

void touch_setup();
void touch_loop();

void screen_setup();
void screen_update();

typedef bool(*InteractionCallback)(String, int, int);

typedef struct IH {
    InteractionCallback callback;
    int minx;
    int maxx;
    int miny;
    int maxy;
} InteractionHandler;

class Screen {
    public:
        int prevIndex;
        TFT_eSprite* spr;
        virtual void init(TFT_eSprite* spr, int width, int height) = 0;
        virtual void update() = 0;
        virtual void render() = 0;
        virtual void registerInteractionHandler(InteractionCallback callback, int minx, int maxx, int miny, int maxy) = 0;
        virtual void registerInteractionHandler(InteractionHandler ih) = 0;
        virtual void handleInteraction(String gesture, int x, int y) = 0;
    protected:
        InteractionHandler interactionHandlers[64]; // arbitrary limit!
        TFT_eSPI* display;
        int iHLen;

};

#endif