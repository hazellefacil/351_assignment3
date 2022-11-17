#include "beatGenerator.h"
#include "LED.h"
#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <alloca.h> // needed for mixer


static char *drumCC = "wave-files/100053__menegass__gui-drum-cc.wav";
static char *tomHiHard = "wave-files/100062__menegass__gui-drum-tom-hi-hard.wav";
static char *snareHard = "wave-files/100058__menegass__gui-drum-snare-hard.wav";
static char *baseHard = "wave-files/100051__menegass__gui-drum-bd-hard.wav";
static char *cyHard = "wave-files/100056__menegass__gui-drum-cyn-hard.wav";
#include "intervalTimer.h"

// have a static int that is bpm
static long long halfBeatinMilliSeconds = 500;
static int currentBPM;
static pthread_t beatThreadId;
static int beatMode;

// private globals
 static wavedata_t globalSnare;
 static wavedata_t globalBase;
 static wavedata_t globalHiHat;

 static wavedata_t globalSnareSt;
 static wavedata_t globalBaseSt;
 static wavedata_t globalHiHatSt;
 static wavedata_t globalCyHard;
 static wavedata_t globalTomHiHard;


static bool keepRunning;

int getMode() {
    return beatMode;
}

int getBPM() {
    return currentBPM;
}

void playSnare() {
    AudioMixer_readWaveFileIntoMemory(snareHard, &globalSnare);  
    AudioMixer_queueSound(&globalSnare);
    sleep(1);
}

void playHiHat(){
        AudioMixer_readWaveFileIntoMemory(drumCC, &globalHiHat);         
        AudioMixer_queueSound(&globalHiHat);
        sleep(1);
        
}

void playBase() {
    AudioMixer_readWaveFileIntoMemory(baseHard, &globalBase);  
    AudioMixer_queueSound(&globalBase);
    sleep(1);
}

static void playStandardRockBeat() 
{
    // hi hat is metallic ting 
    AudioMixer_readWaveFileIntoMemory(cyHard, &globalHiHatSt);
    // snare is sharp middle sound
    AudioMixer_readWaveFileIntoMemory(snareHard, &globalSnareSt);
    // base is hard
    AudioMixer_readWaveFileIntoMemory(baseHard, &globalBaseSt);

        while(beatMode == 1){
            AudioMixer_queueSound(&globalHiHatSt);
            AudioMixer_queueSound(&globalBaseSt);
            sleepForMs(halfBeatinMilliSeconds);
            AudioMixer_queueSound(&globalHiHatSt);
            sleepForMs(halfBeatinMilliSeconds);
            AudioMixer_queueSound(&globalHiHatSt);
            AudioMixer_queueSound(&globalSnareSt);
            sleepForMs(halfBeatinMilliSeconds);
            AudioMixer_queueSound(&globalHiHatSt);
            sleepForMs(halfBeatinMilliSeconds);
            Interval_markInterval(1);
        }
}



static void playCustomBeat() 
{
    // hi hat is metallic ting 
    AudioMixer_readWaveFileIntoMemory(cyHard, &globalHiHatSt);
    // snare is sharp middle sound
    AudioMixer_readWaveFileIntoMemory(snareHard, &globalSnareSt);
    // base is hard
    AudioMixer_readWaveFileIntoMemory(baseHard, &globalBaseSt);

    AudioMixer_readWaveFileIntoMemory(tomHiHard, &globalTomHiHard);

    while(beatMode == 2){
        //first bar of sound
        AudioMixer_queueSound(&globalSnareSt);
        AudioMixer_queueSound(&globalHiHatSt);
        sleepForMs(halfBeatinMilliSeconds);
        AudioMixer_queueSound(&globalBaseSt);
        sleepForMs(halfBeatinMilliSeconds);
        AudioMixer_queueSound(&globalBaseSt);
        sleepForMs(halfBeatinMilliSeconds);
        AudioMixer_queueSound(&globalBaseSt);
        sleepForMs(halfBeatinMilliSeconds);
        //second bar
        AudioMixer_queueSound(&globalTomHiHard);
        sleepForMs(halfBeatinMilliSeconds);
        AudioMixer_queueSound(&globalBaseSt);
        sleepForMs(halfBeatinMilliSeconds);
        AudioMixer_queueSound(&globalSnareSt);
        sleepForMs(halfBeatinMilliSeconds);
        AudioMixer_queueSound(&globalHiHatSt);
        sleepForMs(halfBeatinMilliSeconds);
        Interval_markInterval(1);
    }
}


void sleepForMs(long long delayInMs)
{
 const long long NS_PER_MS = 1000 * 1000;
 const long long NS_PER_SECOND = 1000000000;
 long long delayNs = delayInMs * NS_PER_MS;
 int seconds = delayNs / NS_PER_SECOND;
 int nanoseconds = delayNs % NS_PER_SECOND;
 struct timespec reqDelay = {seconds, nanoseconds};
 nanosleep(&reqDelay, (struct timespec *) NULL);
}


static void* playBeatThread(void* _arg) {
    // DEPENDING ON WHAT THE JOY STICK CALLS WE WILL DO SOMETHANNNG IN HERE
    //playStandardRockBeat();
     while(keepRunning = true) {
        if(beatMode == 0) {
            // do nothing
            displayMode(beatMode);
        }
        else if (beatMode == 1) {
            // rock beat
        displayMode(beatMode);
        playStandardRockBeat();

        }
        else if(beatMode == 2) {
            // play our custom beat
            displayMode(beatMode);
            playCustomBeat();
        }
    }
    return NULL;
}

void beatGenerator_init() {
    setBPM(120);
    beatMode = 1;
    pthread_create(&beatThreadId, NULL, playBeatThread, NULL);
}

void beatGenerator_clean() {
    keepRunning = false;
    pthread_join(beatThreadId, NULL);
    free(&globalBase);
    free(&globalHiHat);
    free(&globalSnare);
    free(&globalSnareSt);
    free(&globalBaseSt);
    free(&globalHiHatSt);
    free(&globalCyHard);
    free(&globalTomHiHard);
}

void setMode(int mode) {
    beatMode = mode;
}

void setBPM(long long bpm) {
    long long halfBeat = 60 / (bpm / 2);
    halfBeat = halfBeat * 1000;
    halfBeatinMilliSeconds = halfBeat;
    currentBPM = (int)bpm;
}



