#ifndef I2C
#define I2C

#include <util/twi.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "definitions.h"

void i2c_init(void);

void i2c_start(void);

void i2c_stop(void);

void i2c_send_data(uint8_t data);

ISR(TWI_vect);

void i2c_write_reg(uint8_t device_address, uint8_t reg_addr, uint8_t data, int n);

uint8_t i2c_read_reg(uint8_t device_address, uint8_t reg_addr, int n);

#endif