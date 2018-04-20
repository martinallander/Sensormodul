/*
 * spi_slav.h
 *
 * Created: 4/20/2018 11:18:43 AM
 *  Author: ludju571
 */ 

/*
 * Kandidatmodul1.c
 *
 * Created: 2018-03-20 13:37:44
 * Author : theod
 */ 

#define ERROR 0x37
#define ACC_OK 0x59
#define TOF_OK 0xBE
#define IR_OK 0xA2
#define MIC_OK 0xE6

#define ACCSIZE 12
#define TOFSIZE 2
#define IRSIZE 64


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void spi_init(void);

void data_direction_init();

//Function to send and receive data
unsigned char spi_tranceiver (unsigned char indata);

void miso_test (uint16_t i);

void mosi_test (uint16_t i);

void spi_test(unsigned int spi_data);

void led_blinker(uint8_t times);
