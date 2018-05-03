/*
 * optiskavst.c
 *
 * Created: 4/24/2018 10:08:37 AM
 *  Author: perli397
 */ 


/*************************************************************************
		Optisk avståndsmätare GP2Y0A21YK 10 - 80 cm
***************************************************************************/

//#include "lcd.h"
//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <util/delay.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>

#include "optiskavst.h"

volatile uint16_t digital_data;
volatile float distance_value;
//volatile char buffer [3];

//för att minska strömanvändningen se DIDR0 s.259
/************************************************************************
	Vi ska använda Free Running mode, genom att använda ADC interupt Flag
	som trigger source!
************************************************************************/

void init_distance(void)
{
	//sätt ADATE för Free Running Mode
	ADCSRA = (1<<ADEN) | (1<<ADIE) | (1<<ADPS1) | (1<<ADPS0);
	return 0;
}

void measure_distance(void)
{
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADIE);
	return 0;
}

//void Write_data_to_LCD(int data)
//{
	//LCD_Clear();
	//LCD_String("Distance:");
	//LCD_Command(0x14);
	//itoa (data, buffer, 10);
	//LCD_String(buffer);
	//LCD_Command(0x14);
	//LCD_String("cm");
	//return 0;
//}

float format_distance(uint16_t unformated_data)
{
	//kalibrering v.1.0
	//float formated_data = (float)((18265)*(pow(unformated_data,-1.226)));
	//Kalibrering v.1.1
	//float formated_data = (float)((10879)*(pow(unformated_data,-1.122)));
	//kalibrering v.1.2 avrundat
	//float formated_data = (float)((10991)*(pow(unformated_data,-1.124)));
	//kalibrering v. 1.3
	float formated_data = (float)((7*pow(10,-9))*(pow(unformated_data,4)) +
								 (-1*pow(10,-5)) * (pow(unformated_data,3)) +
								  (0.0056)*(pow(unformated_data,2)) +
								   (-1.4198)*unformated_data + 
								   158.22);
	return formated_data;
}

ISR(ADC_vect)
{
	digital_data = (uint16_t)(ADCL | (ADCH << 8));
	/*********************************************************
						Vid kalibrering:
	*********************************************************/
	//Write_data_to_LCD(digital_data); 
	/*------------------------------------------------------*/
	distance_value = format_distance(digital_data);
	//Write_data_to_LCD(distance); 
	_delay_ms(1);
	ADCSRA |= (0 << ADEN) | (0 << ADIE);
}

void get_distance(Sensor_Data* sd)
{
	measure_distance();
	_delay_ms(5);
	sd->distance = distance_value;
	return 0;
}
//int main(void)
//{
	//LCD_Init();
	//LCD_Clear();
	//LCD_String("Device's On");
	//_delay_ms(2000);
	//init_AD_conv();
	//LCD_Clear();
	//sei();
	//ADCSRA |= (1 << ADSC);
	//
    //while(1)
	//{}
	//
	//return 0;
//}

