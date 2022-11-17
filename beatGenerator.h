#ifndef BEAT_GEN_H
#define BEAT_GEN_H

#include "audioMixer_template.h"

// thread functions
void beatGenerator_init();
void beatGenerator_clean();
// gpio functions
void setMode(int mode);
void setBPM(long long bpm);
// calculation n stuff
void sleepForMs(long long delayInMs);
int getMode();
int getBPM();
// play sounds
void playBase();
void playHiHat();
void playSnare();
#endif