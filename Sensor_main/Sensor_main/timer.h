#include <avr/io.h>
#include <avr/interrupt.h>
#include "definitions.h"

#ifndef TIMER_H
#define TIMER_H

ISR(TIMER1_OVF_vect);
void timer_1_init(float prescaler);
void timer_1_start();
float timer_1_get_time();

#endif