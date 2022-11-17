
//continuously read button press and joystick 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>


#include "userInput.h"
#include "beatGenerator.h"
#include "LED.h"


#define MODE_BUTTON_PATH "/sys/class/gpio/gpio47/value"
#define BASE_BUTTON_PATH "/sys/class/gpio/gpio46/value"
#define SNARE_BUTTON_PATH "/sys/class/gpio/gpio27/value"
#define HIHAT_BUTTON_PATH "/sys/class/gpio/gpio65/value"


#define A2D_FILE_VOLTAGE2 "/sys/bus/iio/devices/iio:device0/in_voltage2_raw"
#define A2D_FILE_VOLTAGE3 "/sys/bus/iio/devices/iio:device0/in_voltage3_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

static _Bool stopping = false;
static pthread_t inputReadThreadId;
static long long debounceWaitMs = 50;
static long long joystickWait = 250;
static int prevVal = 0;
static _Bool isJoystickHeld = false;

//button is active low
//return value


// int main(void)
// {

//     pthread_create(&inputReadThreadId, NULL, inputReadThread, NULL);
//     while(1){
//         printf("Enter 'Q' to quit.\n");
//         if (toupper(getchar()) == 'Q') {
//             stopping = true;
//             userInput_cleanup();
//             exit(0);
//         }
//     }
// }

void userInput_init() {
    pthread_create(&inputReadThreadId, NULL, inputReadThread, NULL);
}

void buttonController()
{
 
    if(readButtonValue(MODE_BUTTON_PATH) == 1){
        modeButton();
        
    }
    if (readButtonValue(BASE_BUTTON_PATH) == 1){
        baseButton();
    }

    if (readButtonValue(SNARE_BUTTON_PATH) == 1){
        snareButton();
        
    }

    if (readButtonValue(HIHAT_BUTTON_PATH) == 1){
        hiHatButton();
    }
}

void joyStickController()
{
    int volume = AudioMixer_getVolume(); // getVolume
    int BPM = getBPM(); //getBPM
    int changeBy = 5;

    int Reading = getXAxisReading();
    int yReading = getYAxisReading();
    double mapped = (2*(double) Reading-A2D_MAX_READING )/A2D_MAX_READING ;
    double mappedY = (2*(double)yReading-A2D_MAX_READING )/A2D_MAX_READING ;

        if(mapped >0.8){//down 

            if(isJoystickHeld == false){

                sleepForMs(joystickWait);
                volume -= changeBy;

                if(volume < 0){
                    volume = 0;
                }

                AudioMixer_setVolume(volume);
                display(volume);
                sleep(1);
                
                isJoystickHeld = true;

            }else{
                isJoystickHeld = false;
            }
        }

        if(mapped <-0.8){ //up 

            if(isJoystickHeld == false){

                sleepForMs(joystickWait);
                volume += changeBy;

                if(volume > 100){
                    volume = 100;
                }

                AudioMixer_setVolume(volume);
                display(volume);
                sleep(1);
                isJoystickHeld = true;

            } else{
                isJoystickHeld = false;
            }
        }

        if(mappedY > 0.5){ //left (decrease)
            

            if(isJoystickHeld == false){
                sleepForMs(joystickWait);
                BPM -= changeBy;

                if(BPM< 40){
                    BPM = 40;
                }
                setBPM((long long)BPM);
                display(BPM / 60);
                sleep(1);
                isJoystickHeld = true;
            } else{
                isJoystickHeld = false;
            }

        }

        if(mappedY < -0.5){ //right (increase)

            if(isJoystickHeld == false){
                sleepForMs(joystickWait);
                BPM += changeBy;

                if(BPM > 300){
                    BPM = 300;
                }

                setBPM((long long)BPM);
                display(BPM / 60);
                sleep(1);
                isJoystickHeld = true;
            } else{
                isJoystickHeld = false;
            }

        }
        displayMode(getMode());

}


int readButtonValue(char * BUTTON_PATH)
{
    char asciiZero = 48;

    FILE *pButtonValue= fopen(BUTTON_PATH, "r");

    if(pButtonValue== NULL){
        printf("ERROR OPENING %s.", BUTTON_PATH); 
        exit(0);
    }

    const int MAX_LENGTH  = 1024;
    char val[MAX_LENGTH];
    fgets(val, MAX_LENGTH, pButtonValue);


    fclose(pButtonValue);

    if(val[0] == asciiZero){
        return 0;
    }
    return 1;
}

int getXAxisReading()
{
    // Open file
        FILE *f = fopen(A2D_FILE_VOLTAGE2, "r");

        if (!f) {
            printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
            printf(" Check /boot/uEnv.txt for correct options.\n");
            exit(-1);
        }
        // Get reading
        int a2dReading = 0;
        int itemsRead = fscanf(f, "%d", &a2dReading);

        if (itemsRead <= 0) {
            printf("ERROR: Unable to read values from voltage input file.\n");
            exit(-1);
        }
        // Close file
        fclose(f);
        return a2dReading;
}

int getYAxisReading()
{
    // Open file
        FILE *f = fopen(A2D_FILE_VOLTAGE3, "r");
        if (!f) {
            printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
            printf(" Check /boot/uEnv.txt for correct options.\n");
            exit(-1);
        }
        // Get reading
        int a2dReading = 0;
        int itemsRead = fscanf(f, "%d", &a2dReading);

        if (itemsRead <= 0) {
            printf("ERROR: Unable to read values from voltage input file.\n");
            exit(-1);
        }
        // Close file
        fclose(f);
        return a2dReading;
 }



void userInput_cleanup(void)
{
        pthread_join(inputReadThreadId, NULL);
        
}

void modeButton(){
    
    if(readButtonValue(MODE_BUTTON_PATH)) {
        sleepForMs(debounceWaitMs);
        if (getMode() == 2) {
            if(prevVal == 0){
                setMode(0);
                prevVal = 1;
            } else {
                prevVal = 0;
            }
        }
        else {
            if(prevVal == 0){
                setMode(getMode() + 1);
                prevVal = 1;
            } else {
                prevVal = 0;
            }

        }
    }
} 

void baseButton(){
    
    sleepForMs(debounceWaitMs);
    if(readButtonValue(BASE_BUTTON_PATH)){
        if(prevVal == 0){
            printf("play base\n"); //playbase
            playBase();
            prevVal = 1;
        }
    } 
    else {
        prevVal = 0;
    }
}

void snareButton(){
    
    sleepForMs(debounceWaitMs);
    if(readButtonValue(SNARE_BUTTON_PATH)){
        if(prevVal == 0){
            printf("play snare\n"); //play snare
            playSnare();
            prevVal = 1;
        }
    } else {
        prevVal = 0;
    }
}


void hiHatButton(){
    
    sleepForMs(debounceWaitMs);
    if(readButtonValue(HIHAT_BUTTON_PATH)){
        if(prevVal == 0){
            printf("play hiHat\n"); //play hiHat
            playHiHat();
            prevVal = 1;
        }
    } 
    else {
        prevVal = 0;

    }
}

void* inputReadThread(void* _arg)
{
	while (!stopping) {


        buttonController();
        joyStickController();
       
        // Quit?
  
        
	}

	return NULL;
}


