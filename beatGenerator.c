#include "beatGenerator.h"
#include "audioMixer_template.h"
#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <alloca.h> // needed for mixer



void playOtherRockBeat()
{

    char *fileToPlay = "wave-files/100053__menegass__gui-drum-cc.wav";

	wavedata_t sample;

    AudioMixer_init();
    AudioMixer_readWaveFileIntoMemory(fileToPlay, &sample);


}

void applySuperposition()
{

    AudioMixer_init();

    char *fileToPlay1 = "wave-files/100053__menegass__gui-drum-cc.wav";
    char *fileToPlay2 = "wave-files/100062__menegass__gui-drum-tom-hi-hard.wav";

    wavedata_t psound1;
    wavedata_t psound2;
    wavedata_t resultingSound;


    AudioMixer_readWaveFileIntoMemory(fileToPlay1,&psound1);
    AudioMixer_readWaveFileIntoMemory(fileToPlay2,&psound2);

    
    int * resultingData = NULL;
    resultingData = (int *) malloc(psound1.numSamples * sizeof(int));

    printf("this is from the first sound: %u \n",psound1.pData[0] );
    printf("this is from the first sound: %u \n",psound2.pData[0] );

    printf("this is short: %u \n",SHRT_MAX);

    for(int i =0; i < psound1.numSamples; i++){
    
        short toAdd =  (psound1.pData[i] + psound2.pData[i]);
        if( toAdd > SHRT_MAX){
            resultingData[i] = SHRT_MAX;
        }
        if( toAdd < SHRT_MIN){
            resultingData[i] = SHRT_MIN;
        }else{
            resultingData[i] = psound1.pData[i] + psound2.pData[i];
        }

    // printf("this is from the resulting sound: %d\n", resultingData[i]);
    }

    resultingSound.pData = (short *) malloc(psound1.numSamples * sizeof(short));
    // short temp = (short *) resultingData;
    resultingSound.pData = (short *) resultingData;
    resultingSound.numSamples = psound1.numSamples;
    printf("%d", psound1.numSamples);


    AudioMixer_queueSound(&resultingSound);
    sleep(2);

}