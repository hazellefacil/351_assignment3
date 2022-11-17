#ifndef USERINPUT_HEADER
# define USERINPUT_HEADER

void* inputReadThread(void* arg);
void buttonController();
int readButtonValue(char * BUTTON_PATH);
int getXAxisReading();
int getYAxisReading();
void joyStickController();
void userInput_cleanup(void);
void modeButton();
void baseButton();
void snareButton();
void hiHatButton();
void userInput_init();



#endif