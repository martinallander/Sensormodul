#include "timer.h"

volatile int overflows;

ISR(TIMER1_OVF_vect)
{
	//keep track of number of overflows
	overflows++;
}

void timer1_init(uint8_t prescaler)
{
	//normal mode
	TCCR1A |= (0 << WGM10)|(0 << WGM11)|(0 << WGM12);;
	if (prescaler == 0)
	{
		TCCR1B |= (1 << CS00)|(0 << CS01)|(0 << CS02);
	}
	if (prescaler == 8)
	{
		TCCR1B |= (0 << CS00)|(1 << CS01)|(0 << CS02);
	}
	if (prescaler == 64)
	{
		TCCR1B |= (1 << CS00)|(1 << CS01)|(0 << CS02);
	}
	if (prescaler == 256)
	{
		TCCR1B |= (0 << CS00)|(0 << CS01)|(1 << CS02);
	}
	if (prescaler == 1024)
	{
		TCCR1B |= (1 << CS00)|(0 << CS01)|(1 << CS02);
	}
	
	TCNT1 = 0;
	TIMSK1 |= (1 << TOIE1);
	overflows = 0;
}

int get_overflows(void)
{
	return overflows;
}