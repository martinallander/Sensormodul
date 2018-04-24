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
		Optisk avst�ndsm�tare GP2Y0A21YK 10 - 80 cm
***************************************************************************/

/*s�tt max v�rdet f�r volt-in i AREF (minus 1 LSB).
�r det VREF?
AREF �r en pin som (jag tror) maxv�rdet ska kopplas...

!! Koppla 5V till AREF !!
*/

//differential gain s�tts i MUX-bitarna i ADMUX

//resultatet hamnar i registerna ADCH och ADCL. s. 242

//Vilket sampling-mode ska vi anv�nda? s.242-243

/*ska vi ha timead sampling? s. 243-244
svar: vi vill sampla kontinuerligt och sedan h�mta data typ var 10sek
 �r det AutoTriggering??
*/

//Hur v�ljer man input channels och vilka ska vi ha?

//f�r att minska str�manv�ndningen se DIDR0 s.259

void init_AD_conv(void)
{
	/*enamble v�nster sorterat resultat. s. 242
	H�r v�ljer VReF om jag f�rst�r r�tt.
	*/
	ADMUX = 1<<ADLAR;
	
	//Man kan v�lja vad som �r avbrottstriggern genom att s�tta
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