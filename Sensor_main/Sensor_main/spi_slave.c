/*
 * Kandidatmodul1.c
 *
 * Created: 2018-03-20 13:37:44
 * Author : theod
 */ 

#include "spi_slave.h"

void spi_init(void)
{
	SPCR |= (1 << SPIE);  
	SPCR |= (1 << SPE);  //Enable SPI
}

//Function to send and receive data
unsigned char spi_tranceiver (unsigned char indata)
{	
	SPDR = indata;											//Load data into buffer
	while(!(SPSR & (1 << SPIF)));							//Wait until transmission complete
	return SPDR;											//Return received data
}

void data_direction_init()
{
	DDRB = (1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB6);			//Set PB0, PB1, PB2 and PB6 (MISO) as outputs
}