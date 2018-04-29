/*
 * optiskavst.c
 *
 * Created: 4/24/2018 10:08:37 AM
 *  Author: perli397
 */ 


/*************************************************************************
		Optisk avståndsmätare GP2Y0A21YK 10 - 80 cm
***************************************************************************/

#include "lcd.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

volatile uint16_t digital_data;
volatile int distance;
volatile char/* * */ buffer [4] = {0, 0, 0, 0};
 


/*sätt max värdet för volt-in i AREF (minus 1 LSB).
Är det VREF?
AREF är en pin som (jag tror) maxvärdet ska kopplas...

!! Koppla 5V till AREF !!
*/

//differential gain sätts i MUX-bitarna i ADMUX

//resultatet hamnar i registerna ADCH och ADCL. s. 242


/*ska vi ha timead sampling? s. 243-244
svar: vi vill sampla kontinuerligt och sedan hämta data typ var 10sek
 Är det AutoTriggering eller free-running mode??
*/

//Hur väljer man input channels och vilka ska vi ha?

//för att minska strömanvändningen se DIDR0 s.259
/************************************************************************
	Vi ska använda Free Running mode, genom att använda ADC interupt Flag
	som trigger source! check!
************************************************************************/

void init_AD_conv(void)
{
	/*enable vänster sorterat resultat. s. 242
	Det är i ADMUX som vi väljer VReF om jag förstår rätt.
	*/
	//Tror inte vi ska vänsterskifta datan.
	//ADMUX = 1<<ADLAR;
	
	/*
	enable ADC, Auto Trigger och sätter Free Running mode,
	 avbrott och sätter prescaler bits.
	 Osäker på ADATE
	 !!! Tar bort ADATE !!!
	*/
	//DDRA = 0x00;
	ADCSRA = (1<<ADEN) |(1 << ADATE) | (1<<ADIE) | (1<<ADPS1) | (1<<ADPS0);
	
	//Inte samma funktionalitet i 1284P som i 16...
	//MCUCR = 1<<SM0 | 1<<ISC01 | 1<<ISC00;
	return 0;
}

void Write_data_to_LCD(int data)
{
	//char buffer [16];
	//cli();
	LCD_Clear();
	LCD_String("Distance:");
	LCD_Command(0x14);
	//char lo = data & 0xFF;
	//char hi = data >> 8;
	//buffer[0] = lo;
	//buffer[1] = hi;
	
	/*
	 Här sätts data_out till buffer[1]!! Varför?! HUUUUR?!?!?!?
	Tror det hade med längden av buffer.
	
	NYTT MÖJLIGT PROBLEM: Write_data_to_LCD skriver ut en extra "siffra" när
	t.ex. buffer[3] = 0....
	*/
	itoa (data, buffer, 10);
	//sprintf(buffer, "%1f", data);
	LCD_String(buffer);
	LCD_Command(0x14);
	LCD_String("cm");
	//sei();
	return 0;
}

double format_digital_data(uint16_t unformated_data)
{
	//uint16_t formated_data = (-0.0243)*pow(unformated_data,3) + (0.8741)*pow(unformated_data,2) - (0.8226)*unformated_data + (9.1667);
	//double formated_data = (double)((0.4735)*pow(unformated_data,2) + (2.6705)*unformated_data + (7.0833));
	double formated_data = (double)(18265*pow(unformated_data,-1.226));
	return formated_data;
}

int main(void)
{
	//volatile uint16_t data_out;
	//volatile char buffer [16];
	LCD_Init();
	LCD_Clear();
	LCD_String("Device's On");
	_delay_ms(2000);
	init_AD_conv();
	LCD_Clear();
	sei();
	ADCSRA |= (1 << ADSC);
	
    while(1)
	{}
	
	return 0;
}

/*
		Det som typ återstår är förstå hur Free Running mode fungerar
*/
ISR(ADC_vect)
{
	//OBS! ADCL ska läsas först sen ADCH, Görs det här???
//	ADCSRA = (1<<ADEN) |(1 << ADATE) | (0 << ADIE) | (1 << ADPS1) | (1 << ADPS0);;
	digital_data = (uint16_t)(ADCL | (ADCH << 8));
	/*********************************************************/
	//data_out = ADCL;
	//data_out = (uint16_t)(data_out | (ADCH << 8));
	/*********************************************************/
	//data_out = ADC;
	distance = (int)(format_digital_data(digital_data));
	Write_data_to_LCD(distance);
	_delay_ms(500);
//	ADCSRA |= (1 << ADIE);
	//hoppar man ur avbrottet direkt efter läsningen av ADCH 
	//eller måste man göra detta?
	//ADCSRA = (0 << ADEN) | (0 << ADIE);
	
}