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

volatile uint8_t data_out;
volatile char buffer [8];


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
	ADMUX = 1<<ADLAR;
	
	/*
	enable ADC, Auto Trigger och sätter Free Running mode,
	 avbrott och sätter prescaler bits.
	 Osäker på ADATE
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
		Det som typ återstår är förstå hur Free Running mode fungerar
*/
ISR(ADC_vect)
{
	//OBS! ADCL ska läsas först sen ADCH, Görs det här???
	//data_out = (uint16_t)((ADCH << 8) | ADCL);
	data_out = ADCH;
	//kan bli så att den aldrig hamnar i Write_to_LCD...
	Write_data_to_LCD(data_out);
	_delay_ms(500);
	//hoppar man ur avbrottet direkt efter läsningen av ADCH 
	//eller måste man göra detta?
	ADCSRA = (0 << ADEN) | (0 << ADIE);
	
}