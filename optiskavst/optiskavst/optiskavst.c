/*
 * optiskavst.c
 *
 * Created: 4/24/2018 10:08:37 AM
 *  Author: perli397
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*************************************************************************
		Optisk avståndsmätare GP2Y0A21YK 10 - 80 cm
***************************************************************************/

/*sätt max värdet för volt-in i AREF (minus 1 LSB).
Är det VREF?
AREF är en pin som (jag tror) maxvärdet ska kopplas...

!! Koppla 5V till AREF !!
*/

//differential gain sätts i MUX-bitarna i ADMUX

//resultatet hamnar i registerna ADCH och ADCL. s. 242

//Vilket sampling-mode ska vi använda? s.242-243

/*ska vi ha timead sampling? s. 243-244
svar: vi vill sampla kontinuerligt och sedan hämta data typ var 10sek
 Är det AutoTriggering??
*/

//Hur väljer man input channels och vilka ska vi ha?

//för att minska strömanvändningen se DIDR0 s.259

void init_AD_conv(void)
{
	/*enamble vänster sorterat resultat. s. 242
	Här väljer VReF om jag förstår rätt.
	*/
	ADMUX = 1<<ADLAR;
	
	//Man kan välja vad som är avbrottstriggern genom att sätta
	//bitarna ADTS i ADCSRB
	ADCSRA = 1<<ADEN | 1<<ADIE | 1<<ADPS1 | 1<<ADPS0;
	
	//Inte samma funktionalitet i 1284P som i 16...
	//MCUCR = 1<<SM0 | 1<<ISC01 | 1<<ISC00;

}

int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code 
    }
}