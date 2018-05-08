#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void led_blink_red (uint16_t i);

void led_blink_green (uint16_t i);

void led_blink_yellow (uint16_t i);

void led_blink_red_2 (uint16_t i);
