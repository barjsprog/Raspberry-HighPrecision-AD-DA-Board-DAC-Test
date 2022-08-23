#include "DAC8532.h"
#include <wiringPi.h>
#include <DEV_Config.h>
#include <wiringPiSPI.h>
#include <signal.h>   
#include <time.h>
#include <stdlib.h>  

static void DEV_GPIOConfig(void)
{
    pinMode(DEV_CS_PIN, OUTPUT);
}

int DEV_ModuleInit(void)
{
    //1.wiringPiSetupGpio
    //if(wiringPiSetup() < 0)//use wiringpi Pin number table
    if(wiringPiSetupGpio() < 0) { //use BCM2835 Pin number table
        Debug("set wiringPi lib failed	!!! \r\n");
        return 1;
    } else {
        Debug("set wiringPi lib success  !!! \r\n");
    }


    //2.GPIO config
    DEV_GPIOConfig();
	
    //3.spi init
    //wiringPiSPISetup(0,3200000,1);
	wiringPiSPISetupMode(0,3200000,1);
    return 0;
}

void SPI_WriteByte(uint8_t value)
{
    int read_data;
    read_data = wiringPiSPIDataRW(0,&value,1);
    if(read_data < 0)
        perror("wiringPiSPIDataRW failed\r\n");
}



UBYTE SPI_ReadByte()
{
    UBYTE read_data,value=0xff;
    read_data = wiringPiSPIDataRW(0,&value,1);
    if(read_data < 0)
        perror("wiringPiSPIDataRW failed\r\n");
    return value;
}

void DEV_ModuleExit(void)
{

}

static void Write_DAC8532(UBYTE Channel, UWORD Data)
{
    DEV_Digital_Write(DEV_CS_PIN, 1);
    DEV_Digital_Write(DEV_CS_PIN, 0);
    DEV_SPI_WriteByte(Channel);
    DEV_SPI_WriteByte((Data>>8));
    DEV_SPI_WriteByte((Data&0xff));  
    DEV_Digital_Write(DEV_CS_PIN, 1);
}

void DAC8532_Out_Voltage(UBYTE Channel, float Voltage)
{
    UWORD temp = 0;
    if((Voltage <= DAC_VREF) && (Voltage >= 0)){
        temp = (UWORD)(Voltage * DAC_Value_MAX / DAC_VREF);
        Write_DAC8532(Channel, temp);
    }
}

void  Handler(int signo)
{
    //System Exit
    printf("\r\nEND                  \r\n");
    DEV_ModuleExit();

    exit(0);
}

int main(void)
{
  
    DEV_ModuleInit();
    signal(SIGINT, Handler);

    printf("Program start\r\n");

    DAC8532_Out_Voltage(channel_A,3.3);  #voltage put here
	DAC8532_Out_Voltage(channel_B,0);
    return 0;
}

// run gcc -Wall -o blinkdeneme blinking_test.c -lwiringPi - 
