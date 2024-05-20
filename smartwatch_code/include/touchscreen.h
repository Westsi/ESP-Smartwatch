#ifndef TOUCH_SCREEN_H_
#define TOUCH_SCREEN_H_

#include <CST816S.h>
#include <TFT_eSPI.h>

typedef void(*InteractionCallback)(String gesture, int x, int y);

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

enum AnimationSelect {
    ANIMATE_IN_FROM_RIGHT,
    ANIMATE_IN_FROM_LEFT
};

void touch_setup();
void touch_loop();

void screen_setup();
void screen_update();
void switchScr(Screen* new_screen);
void animateSwitch(AnimationSelect as, Screen* os, Screen* ns);

// Fonts

#include "fonts/Roboto_Light12.h"
#define FontLight12 Roboto_Light12

#include "fonts/Roboto_Light14.h"
#define FontLight14 Roboto_Light14

#include "fonts/Roboto_Light16.h"
#define FontLight16 Roboto_Light16

#include "fonts/Roboto_Light18.h"
#define FontLight18 Roboto_Light18

#include "fonts/Roboto_Light20.h"
#define FontLight20 Roboto_Light20

#include "fonts/Roboto_Light24.h"
#define FontLight24 Roboto_Light24

#include "fonts/Roboto_Light28.h"
#define FontLight28 Roboto_Light28



#include "fonts/Roboto12.h"
#define Font12 Roboto12

#include "fonts/Roboto14.h"
#define Font14 Roboto14

#include "fonts/Roboto16.h"
#define Font16 Roboto16

#include "fonts/Roboto18.h"
#define Font18 Roboto18

#include "fonts/Roboto20.h"
#define Font20 Roboto20

#include "fonts/Roboto24.h"
#define Font24 Roboto24

#include "fonts/Roboto28.h"
#define Font28 Roboto28



#endif