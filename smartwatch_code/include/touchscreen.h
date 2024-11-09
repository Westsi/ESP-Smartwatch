#ifndef TOUCH_SCREEN_H_
#define TOUCH_SCREEN_H_

#include <CST816S.h>
#include <TFT_eSPI.h>

typedef void(*InteractionCallback)(String gesture, int x, int y);

extern long timeOfLastInteraction;

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
void turnScreenOff();
void setScreenBrightness(int val);
int getScreenBrightness();
void animateSwitch(AnimationSelect as, Screen* os, Screen* ns);
void recolorImage(const uint16_t* image, int w, int h, int newcol, uint16_t* buf);

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

#include "fonts/Roboto64.h"
#define Font64 Roboto64


#include "fonts/JetBrainsMonoRegular12.h"
#define FontMono12 JetBrainsMonoRegular12

#include "fonts/JetBrainsMonoRegular14.h"
#define FontMono14 JetBrainsMonoRegular14

#include "fonts/JetBrainsMonoRegular16.h"
#define FontMono16 JetBrainsMonoRegular16

#include "fonts/JetBrainsMonoRegular18.h"
#define FontMono18 JetBrainsMonoRegular18

#include "fonts/JetBrainsMonoRegular20.h"
#define FontMono20 JetBrainsMonoRegular20

#include "fonts/JetBrainsMonoRegular24.h"
#define FontMono24 JetBrainsMonoRegular24

#include "fonts/JetBrainsMonoRegular28.h"
#define FontMono28 JetBrainsMonoRegular28

#include "fonts/JetBrainsMonoRegular48.h"
#define FontMono48 JetBrainsMonoRegular48

#include "fonts/JetBrainsMonoRegular64.h"
#define FontMono64 JetBrainsMonoRegular64


#include "fonts/Rubik12.h"
#define FontRubik12 Rubik12

#include "fonts/Rubik14.h"
#define FontRubik14 Rubik14

#include "fonts/Rubik16.h"
#define FontRubik16 Rubik16

#include "fonts/Rubik18.h"
#define FontRubik18 Rubik18

#include "fonts/Rubik20.h"
#define FontRubik20 Rubik20

#include "fonts/Rubik24.h"
#define FontRubik24 Rubik24

#include "fonts/Rubik28.h"
#define FontRubik28 Rubik28

#include "fonts/Rubik48.h"
#define FontRubik48 Rubik48

#include "fonts/Rubik64.h"
#define FontRubik64 Rubik64

#include "fonts/NotoSansMath12.h"
#define FontMath12 NotoSansMath12

#include "fonts/NotoSansMath18.h"
#define FontMath18 NotoSansMath18

#include "fonts/NotoSansMath24.h"
#define FontMath24 NotoSansMath24

#include "fonts/NotoSansMath28.h"
#define FontMath28 NotoSansMath28


#endif