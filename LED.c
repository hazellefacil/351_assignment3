
#include "LED.h"
#include <string.h>

static int numRows = 7;
static int left80 = 0b10000000;
static int left01 = 0b10000001;
static int left02 = 0b10000010;

static int right10 = 0b00010000;
static int right20 = 0b00100000;
static int right40 = 0b01000000;


static int zero[7][3] = {
    {1,1,1},
    {1,0,1},
    {1,0,1},
    {1,0,1},
    {1,0,1},
    {1,0,1},
    {1,1,1}
};

static int one[7][3] = {
    {0,1,0},
    {1,1,0},
    {0,1,0},
    {0,1,0},
    {0,1,0},
    {0,1,0},
    {1,1,1}
};

static int two[7][3] = {
    {1,1,1},
    {0,0,1},
    {0,0,1},
    {1,1,1},
    {1,0,0},
    {1,0,0},
    {1,1,1}
};

static int three[7][3] = {
    {1,1,1},
    {0,0,1},
    {0,0,1},
    {1,1,1},
    {0,0,1},
    {0,0,1},
    {1,1,1}
};

static int four[7][3] = {
    {1,0,1},
    {1,0,1},
    {1,0,1},
    {1,1,1},
    {0,0,1},
    {0,0,1},
    {0,0,1}
};

static int five[7][3] = {
    {1,1,1},
    {1,0,0},
    {1,0,0},
    {1,1,1},
    {0,0,1},
    {0,0,1},
    {1,1,1}
};

static int six[7][3] = {
    {1,1,1},
    {1,0,0},
    {1,0,0},
    {1,1,1},
    {1,0,1},
    {1,0,1},
    {1,1,1}
};

static int seven[7][3] = {
    {1,1,1},
    {0,0,1},
    {0,0,1},
    {0,0,1},
    {0,0,1},
    {0,0,1},
    {0,0,1}
};

static int eight[7][3] = {
    {1,1,1},
    {1,0,1},
    {1,0,1},
    {1,1,1},
    {1,0,1},
    {1,0,1},
    {1,1,1}
};


static int nine[7][3] = {
    {1,1,1},
    {1,0,1},
    {1,0,1},
    {1,1,1},
    {0,0,1},
    {0,0,1},
    {0,0,1}
};

static int letter_m[7][3] = {
    {0,0,1},
    {1,1,1},
    {1,1,1},
    {0,0,1},
    {0,0,1},
    {0,0,1},
    {0,0,1}
};



typedef struct{
    int keyNum; 
    int keyArray[7][3];

} keys;

int initI2cBus(char* bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

void writeI2cReg(int i2cFileDesc, unsigned char regAddr,int values[])
{
    unsigned char buff[17];
    buff[0] = regAddr;

    for(int i = 1; i < 18; i++){
        buff[i] = values[i-1];
    }

    int res = write(i2cFileDesc, buff, 17);

    if (res != 17) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

int * leftDigit(int (*array)[7][3]){

    int *leftBin;
    leftBin = (int*)malloc(numRows * sizeof(int) * 5);
    //int leftBin[7];

    for(int i = 0; i <numRows;i++){
        int binHex80 = 0;
        int binHex02 = 0;
        int binHex01 = 0;

        if((*array)[i][0]==1){
            binHex80 = left80;
        }
        if((*array)[i][1]==1){
            binHex01 = left01;
        }
        if((*array)[i][2]==1){
            binHex02 = left02;
        }
        
        leftBin[i] = binHex80 + binHex01 + binHex02;
    }
    return leftBin;
}

int *rightDigit(int (*array)[7][3]){
    int *rightBin;
    rightBin = (int*)malloc(numRows * sizeof(int) * 5);

    for(int i = 0; i <numRows;i++){
        int binHex10 = 0;
        int binHex20 = 0;
        int binHex40 = 0;

        if((*array)[i][0]==1){
            binHex10 = right10;
        }
        if((*array)[i][1]==1){
            binHex20 = right20;
        }
        if((*array)[i][2]==1){
            binHex40 = right40;
        }
        
        rightBin[i] = binHex10 + binHex20 + binHex40;
    }
    return rightBin;
}

void displayMode(int mode){

    int *right;
    right = rightDigit(chooseDigit(mode));

    int *left;
    left = leftDigit(chooseDigit(10));

    int *toDisplay;
    toDisplay= (int*)malloc(numRows * sizeof(int) * 5);

    for(int i = 0 ; i< numRows;i++){
        toDisplay[i] = left[i] | right[i];
    }

    free(left);
    free(right);

    int valueArray[17];
    for(int i=0; i < 18; i++){
        valueArray[i] = 0x00;
    }

    for(int i=0; i<7; i++){
        valueArray[i*2] = toDisplay[i];
    }
    free(toDisplay);

    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    writeI2cReg(i2cFileDesc, REG_DIRA, valueArray);
    close(i2cFileDesc);
}


void display(int num){
    int leftDigitNum = 0;
    int rightDigitNum = 0;

    if(num<10){
        rightDigitNum = num;
    }else{
        leftDigitNum = num/10;
        rightDigitNum = num % 10; 
    }

    if(num>100){
        rightDigitNum =9;
        leftDigitNum =9;
    }

    int *right;
    // right = (int*)malloc(numRows * sizeof(int) * 5);
    right = rightDigit(chooseDigit(rightDigitNum));

    int *left;
    // left = (int*)malloc(numRows * sizeof(int) * 5);
    left = leftDigit(chooseDigit(leftDigitNum));

    int *toDisplay;
    toDisplay= (int*)malloc(numRows * sizeof(int) * 5);

    for(int i = 0 ; i< numRows;i++){
        toDisplay[i] = left[i] | right[i];
    }

    free(left);
    free(right);

    int valueArray[17];
    for(int i=0; i < 18; i++){
        valueArray[i] = 0x00;
    }

    for(int i=0; i<7; i++){
        valueArray[i*2] = toDisplay[i];
    }
    free(toDisplay);

    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    writeI2cReg(i2cFileDesc, REG_DIRA, valueArray);
    close(i2cFileDesc);
}


void displayFloat(double num){
    int leftDigitNum = 0;
    int rightDigitNum = 0;

    double temp = (num - (int)num) * 10;
    leftDigitNum = (int) num;
    rightDigitNum = (int) temp;

    if(num>9.9) {
        rightDigitNum =9;
        leftDigitNum =9;
    } if(num<0.0) {
        rightDigitNum =0;
        leftDigitNum =0;
    }

    int *right;
    right = (int*)malloc(numRows * sizeof(int) * 5);
    right = rightDigit(chooseDigit(rightDigitNum));

    int *left;
    left = (int*)malloc(numRows * sizeof(int) * 5);
    left = leftDigit(chooseDigit(leftDigitNum));

    int *toDisplay;
    toDisplay= (int*)malloc(numRows * sizeof(int) * 5);

    for(int i = 0 ; i < numRows;i++){
        toDisplay[i] = left[i] | right[i];
    }

    free(left);
    free(right);

    int valueArray[17];
    for(int i=0; i < 18; i++){
        valueArray[i] = 0x00;
    }

    for(int i=0; i<7; i++){
        valueArray[i*2] = toDisplay[i];
    }
    valueArray[14] = 0x08;
    
    free(toDisplay);

    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    writeI2cReg(i2cFileDesc, REG_DIRA, valueArray);
    close(i2cFileDesc);
}

int (* chooseDigit(int num))[7][3]{

    switch(num){
        case 0:
            return &zero;
        case 1:
            return &one;
        case 2:
            return &two;
        case 3:
            return &three;
        case 4:
            return &four;
        case 5:
            return &five;
        case 6:
            return &six;
        case 7:
            return &seven;
        case 8:
            return &eight;
        case 9:
            return &nine;
        case 10:
            return &letter_m;
    }
    return 0;
}















