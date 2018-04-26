/*
 * spi_slav.h
 *
 * Created: 4/20/2018 11:18:43 AM
 *  Author: ludju571
 */ 

/*
 * Kandidatmodul1.c
 *
 * Created: 2018-03-20 13:37:44
 * Author : theod
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "definitions.h"

void spi_init(void);

void data_direction_init();

//Function to send and receive data
unsigned char spi_tranceiver (unsigned char indata);