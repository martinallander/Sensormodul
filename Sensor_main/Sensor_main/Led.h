/*
 * Led.h
 *
 * Created: 4/20/2018 11:41:37 AM
 *  Author: ludju571
 */ 


#ifndef F_CPU
#define F_CPU 16000000UL
#endif


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void led_blinker(uint8_t times);

void led_blink_red (uint16_t i);

void led_blink_green (uint16_t i);

void led_blink_yellow (uint16_t i);
