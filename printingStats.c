#include "audioMixer_template.h"
#include "beatGenerator.h"
#include "LED.h"
#include "intervalTimer.h"
#include "userInput.h"
#include <alsa/asoundlib.h>
#include "LED.h"
#include <stdbool.h>
#include <pthread.h>
#include "printingStats.h"

static pthread_t printingThreadId;
static int stop = 0;

void printing_init() {
    pthread_create(&printingThreadId, NULL, printingStatsThread, NULL);
}

static void* printingStatsThread(void* _arg) 
{
    while(stop == 0)
    {
	    Interval_statistics_t lowLevelStats;
	    Interval_statistics_t beatStats;

		Interval_getStatisticsAndClear(0, &lowLevelStats);
		Interval_getStatisticsAndClear(1, &beatStats);
		printf("M%d ", getMode());
		printf("%dbpm ", getBPM());
		printf("Vol: %d ", AudioMixer_getVolume());
		printf("Low[%f, %f] avg {%f}/{%d} ", lowLevelStats.minIntervalInMs, lowLevelStats.minIntervalInMs, lowLevelStats.avgIntervalInMs, lowLevelStats.numSamples);
		printf("Beat[%f, %f] avg {%f}/{%d} \n", beatStats.minIntervalInMs, beatStats.maxIntervalInMs, beatStats.minIntervalInMs, beatStats.numSamples);
		sleep(1);
    }
        return NULL;
}


void printing_cleanup(void)
{
        stop = 1;
        pthread_exit(printingThreadId);
        pthread_join(printingThreadId,NULL); 
}