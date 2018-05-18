#ifndef UART_H_
#define UART_H_

#ifndef F_CPU
#define F_CPU 16000000UL							//S�tter CPU-klockan till 16 MHz
#endif

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

//vad h�mtar koden fr�n definitions.h egentligen??
//#include "definitions.h"


void send_mode(void);

void uart_init(void);

void uart_transmit(unsigned char data);

unsigned char uart_recieve(void);

#endif /* UART_H_ */