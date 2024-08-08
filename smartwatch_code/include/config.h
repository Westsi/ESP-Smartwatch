#ifndef X_CONFIG_H_
#define X_CONFIG_H_

void initConfig();
int loadBrightness();
void saveBrightness(int brightness);
int loadWatchFace();
void saveWatchFace(int wf);

#define BRIGHTNESS_KEY "brightness"
#define WATCHFACE_KEY "wf"
#define WATCHFACE_DIGITAL 1
#define WATCHFACE_ANALOG 0

#endif