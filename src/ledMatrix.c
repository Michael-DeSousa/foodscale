#include "ledMatrix.h"

void SPIsetup()
{
    DDRB = 0xFF; //PB4 is slave select (sets our matrix), Data is sent through PB5 (Master Output Slave Input), PB7 controls the clock sync
    PORTB |= (1 << PB4);
     
    SPCR = (1 << SPE) | (1 << MSTR); //SPE enables SPI, MSTR sets ATMEGA1284 as master
    return;
}

void sendByte(unsigned char data)
{
    SPDR = data;
    while(!(SPSR & (1 << SPIF)))
    {
        //Wait until everything is sent over
    }
            
    return;
}

void sendData(unsigned char address, unsigned char data)
{
    CS_On;
    sendByte(address);
    sendByte(data);
    CS_Off;
    
    return;
}

void initializeMatrix()
{
    CS_On;
    unsigned char brightness = 0x09; //Can go from min of 0x00 to max of 0x0F
    sendData(0x0A, brightness); //0x0A is address for intensity register
    CS_Off;
    
    CS_On;
    sendData(0x0B, 0x07); //0x0B is address for scan-limit register, 0x07 Displays all 8 columns
    CS_Off;
    
    CS_On;
    sendData(0x0C, 0x01); //0x0C is address for Shutdown (a.k.a power) register, 1 = Normal Operation / 0 = Shutdown
    CS_Off;
    
    CS_On;
    sendData(0x09, 0x00); //0x09 is address for decode mode, we don't use it
    CS_Off;
    
    CS_On;
    sendData(0x0F, 0x00); //0x0F is address for Display Test register, 0 = Normal Operation / 1 = Test Mode
    CS_Off;
    
    return;
}

void clear()
{
    CS_On;
    for(unsigned char i = 1; i < 9; ++i) //NOTE: Though the columns are considered 0-7, the address registers are 1-8
    {
        sendData(i, 0x00);
    }
    CS_Off;
    
    return;
}

void display(unsigned char message)
{
    CS_On;
    for(unsigned char i = 1; i < 9; ++i)
    {
        sendData(i, message);
    }
    CS_Off;
    
    return;
}
