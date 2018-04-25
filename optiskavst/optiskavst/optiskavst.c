/*
 * optiskavst.c
 *
 * Created: 4/24/2018 10:08:37 AM
 *  Author: perli397
 */ 


/*************************************************************************
		Optisk avst�ndsm�tare GP2Y0A21YK 10 - 80 cm
***************************************************************************/

#include "lcd.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

volatile uint8_t data_out;
volatile char buffer [8];


/*s�tt max v�rdet f�r volt-in i AREF (minus 1 LSB).
�r det VREF?
AREF �r en pin som (jag tror) maxv�rdet ska kopplas...

!! Koppla 5V till AREF !!
*/

//differential gain s�tts i MUX-bitarna i ADMUX

//resultatet hamnar i registerna ADCH och ADCL. s. 242


/*ska vi ha timead sampling? s. 243-244
svar: vi vill sampla kontinuerligt och sedan h�mta data typ var 10sek
 �r det AutoTriggering eller free-running mode??
*/

//Hur v�ljer man input channels och vilka ska vi ha?

//f�r att minska str�manv�ndningen se DIDR0 s.259
/************************************************************************
	Vi ska anv�nda Free Running mode, genom att anv�nda ADC interupt Flag
	som trigger source! check!
************************************************************************/

void init_AD_conv(void)
{
	/*enable v�nster sorterat resultat. s. 242
	Det �r i ADMUX som vi v�ljer VReF om jag f�rst�r r�tt.
	*/
	//Tror inte vi ska v�nsterskifta datan.
	ADMUX = 1<<ADLAR;
	
	/*
	enable ADC, Auto Trigger och s�tter Free Running mode,
	 avbrott och s�tter prescaler bits.
	 Os�ker p� ADATE
	*/
	ADCSRA = 1<<ADEN | 1<<ADATE | 1<<ADIE | 1<<ADPS1 | 1<<ADPS0;
	
	//Inte samma funktionalitet i 1284P som i 16...
	//MCUCR = 1<<SM0 | 1<<ISC01 | 1<<ISC00;
	return 0;
}

void Write_data_to_LCD(uint16_t data)
{
	//char buffer [16];
	
	LCD_Clear();
	LCD_String("Distance:");
	LCD_Command(0x14);
	itoa (data, buffer, 10);
	LCD_String(buffer);
	return 0;
}

int main(void)
{
	//volatile uint16_t data_out;
	//volatile char buffer [16];
	int a = 0;
	LCD_Init();
	LCD_Clear();
	LCD_String("Device's On");
	_delay_ms(2000);
	init_AD_conv();
	LCD_Clear();
	sei();
	ADCSRA |= (1 << ADSC) | (1 << ADIF);
    while(1)
    {
        a = a + 1;
		a = a - 1; 
    }
}

/*
		Det som typ �terst�r �r f�rst� hur Free Running mode fungerar
*/
ISR(ADC_vect)
{
	//OBS! ADCL ska l�sas f�rst sen ADCH, G�rs det h�r???
	//data_out = (uint16_t)((ADCH << 8) | ADCL);
	data_out = ADCH;
	//kan bli s� att den aldrig hamnar i Write_to_LCD...
	Write_data_to_LCD(data_out);
	_delay_ms(500);
	//hoppar man ur avbrottet direkt efter l�sningen av ADCH 
	//eller m�ste man g�ra detta?
	ADCSRA = (0 << ADEN) | (0 << ADIE);
	
}