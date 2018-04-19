#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef TIMER_H
#define TIMER_H

ISR(TIMER1_OVF_vect);
void timer1_init(uint8_t prescaler);
int get_overflows(void);

#endif