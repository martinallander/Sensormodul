#include "led.h"

void led_blink_red (uint16_t i)
{
	//Blink LED "i" number of times
	for (; i>0; --i)
	{
		PORTB|=(1<<0);
		_delay_ms(500);
		PORTB=(0<<0);
		_delay_ms(500);
	}
}

void led_blink_green (uint16_t i)
{
	//Blink LED "i" number of times
	for (; i>0; --i)
	{
		PORTB|=(1<<1);
		_delay_ms(500);
		PORTB=(0<<1);
		_delay_ms(500);
	}
}

void led_blink_yellow (uint16_t i)
{
	//Blink LED "i" number of times
	for (; i>0; --i)
	{
		PORTB|=(1<<2);
		_delay_ms(500);
		PORTB=(0<<2);
		_delay_ms(500);
	}
}