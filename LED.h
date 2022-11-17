#ifndef HEADER_FILE
# define HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x70
#define REG_DIRA 0x00

int initI2cBus(char* bus, int address);
void writeI2cReg(int i2cFileDesc, unsigned char regAddr, int values[]);
int * leftDigit(int (*array)[7][3]);
int * rightDigit(int (*array)[7][3]);
void display(int num);
void displayMode(int mode);
void displayFloat(double num);
int (* chooseDigit(int num))[7][3];
void writeToDisplay(int (*array)[7][3]);

//physical LED columns hex ->binary


#endif