/*************************************************************************
		Optisk avståndsmätare GP2Y0A21YK 10 - 80 cm
**************************************************************************/
#ifndef OPTISKAVST
#define OPTISKAVST

#ifndef F_CPU 
#define F_CPU 16000000UL
#endif
/***************************************************************************
						Includes - libraries m.m.
****************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lcd.h"
/*==========================================================================*/


/***************************************************************************
								Variables
****************************************************************************/
//volatile uint16_t digital_data;
//volatile int distance;
//volatile char buffer [3];
/*==========================================================================*/


/***************************************************************************
								Functions
****************************************************************************/

void init_AD_conv(void);

void first_conversion(void);

float format_digital_data(uint16_t unformated_data);

ISR(ADC_vect);

#endif