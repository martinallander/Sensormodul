/*
 * optiskavst.c
 *
 * Created: 4/24/2018 10:08:37 AM
 *  Author: perli397
 */ 


/*************************************************************************
		Optisk avst�ndsm�tare GP2Y0A21YK 10 - 80 cm
***************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint16_t data_out;


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
	//ADMUX = 1<<ADLAR;
	
	/*
	enable ADC, Auto Trigger och s�tter Free Running mode,
	 avbrott och s�tter prescaler bits.
	 Os�ker p� ADATE
	*/
	ADCSRA = 1<<ADEN | 1<<ADATE | 1<<ADIE | 1<<ADPS1 | 1<<ADPS0;
	
	//Inte samma funktionalitet i 1284P som i 16...
	//MCUCR = 1<<SM0 | 1<<ISC01 | 1<<ISC00;
}

int main(void)
{
	init_AD_conv();
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}

/*
		Det som typ �terst�r �r f�rst� hur Free Running mode fungerar
*/
ISR(ADC_vect)
{
	//OBS! ADCL ska l�sas f�rst sen ADCH, G�rs det h�r???
	data_out = (uint16_t)((ADCH << 8) | ADCL);
	//hoppar man ur avbrottet direkt eller m�ste man g�ra detta?
	ADCSRA = (0 << ADEN) | (0 << ADIE)
	
}