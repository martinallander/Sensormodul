#ifndef TIMER_H
#define TIMER_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include "definitions.h"


ISR(TIMER1_OVF_vect);
void timer_1_init(float prescaler);
void timer_1_start();
float timer_1_get_time();

#endif