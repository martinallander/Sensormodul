#ifndef LCD_H
#define LCD_H

#include <inttypes.h>
#include <avr/pgmspace.h>

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define LCD_Dir DDRB					// LCD data port direction
#define LCD_Port PORTB					// LCD data port
#define RS PINB3						// Signalpin för Register Select (data reg./command reg.)
#define EN PINB2 						// Signalpin för Enable

void LCD_Command( unsigned char cmnd );

void LCD_Char( unsigned char data );

void LCD_Init (void);

void LCD_String (char *str);

void LCD_String_xy (char row, char pos, char *str);

void LCD_Clear();

#endif