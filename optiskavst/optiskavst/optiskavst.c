/*
 * optiskavst.c
 *
 * Created: 4/24/2018 10:08:37 AM
 *  Author: perli397
 */ 


/*************************************************************************
		Optisk avståndsmätare GP2Y0A21YK 10 - 80 cm
***************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint16_t data_out;


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
		Det som typ återstår är förstå hur Free Running mode fungerar
*/
ISR(ADC_vect)
{
	//OBS! ADCL ska läsas först sen ADCH, Görs det här???
	data_out = (uint16_t)((ADCH << 8) | ADCL);
	//hoppar man ur avbrottet direkt eller måste man göra detta?
	ADCSRA = (0 << ADEN) | (0 << ADIE)
	
}