#ifndef LEDMATRIX_H
#define LEDMATRIX_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "ledMatrix.h"

#define CS_On PORTB &= ~(1 << PB4) //Sets this high to choose the matrix as the recipient of data
#define CS_Off PORTB |= (1 << PB4) //Sets this low after data is done being sent to deselect

//Sets up the matrix with SPI
void SPIsetup();

//Sends a byte of data from the microcontroller to the matrix, you will almost always use sendData instead of this.
void sendByte(unsigned char data);

//To interface with the matrix you need to send the address of a register first followed by the data you wish to send. Use datasheet for register information.
void sendData(unsigned char address, unsigned char data);

//Initializes the settings of the matrix to default values, e.g. brightness, decode mode... etc.
void initializeMatrix();

//Clears the LEDs on the matrix, ensuring nothing is lit up
void clear();

//Sends a byte to the matrix, telling it which rows to light up. For example, calling display(0b00000011) would light up the first two rows of the matrix.
void display(unsigned char message);

#endif