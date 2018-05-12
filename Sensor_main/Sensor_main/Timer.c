#include "timer.h"

volatile uint64_t overflows;
volatile float prescaler_;

ISR(TIMER1_OVF_vect)
{
	//keep track of number of overflows
	overflows++;
}

void timer_1_init(float prescaler)
{
	//normal mode
	TCCR1A |= (0 << WGM10)|(0 << WGM11)|(0 << WGM12);	//Det är något skumt med timer asså´...
	if (prescaler == 1.0)
	{
		TCCR1B |= (1 << CS00)|(0 << CS01)|(0 << CS02);
	}
	if (prescaler == 8.0)
	{
		TCCR1B |= (0 << CS00)|(1 << CS01)|(0 << CS02);
	}
	if (prescaler == 64.0)
	{
		TCCR1B |= (1 << CS00)|(1 << CS01)|(0 << CS02);
	}
	if (prescaler == 256.0)
	{
		TCCR1B |= (0 << CS00)|(0 << CS01)|(1 << CS02);
	}
	if (prescaler == 1024.0)
	{
		TCCR1B |= (1 << CS00)|(0 << CS01)|(1 << CS02);
	}
	prescaler_ = prescaler;
	TIMSK1 |= (1 << TOIE1);
}

void timer_1_start()
{
	overflows = 0;
	TCNT1 = 0;
}

float timer_1_get_time()
{
	float time = 0.0;
	uint16_t count = TCNT1;
	uint64_t tot_count = (overflows * 65535) + count;
	time = (prescaler_/F_CPU) * (tot_count + 1);
	return time;
}