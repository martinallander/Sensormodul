#ifndef SENSOR_MAIN_H_
#define SENSOR_MAIN_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "definitions.h"
#include "FIFO_Queue.h"
#include "spi_slave.h"
#include "i2c.h"
#include "led.h"
#include "timer.h"
#include "sensor_data.h"

float format_acc(uint8_t low, uint8_t high);

float format_gyro(uint8_t low, uint8_t high);

float format_temp(uint8_t low, uint8_t high);

void init_temp(void);

void init_acc(void);

void init_gyro(void);

void get_temp(Sensor_Data* sd);

void get_acc(Sensor_Data* sd);

void get_gyro(Sensor_Data* sd);

void get_angle(Sensor_Data* sd);

void get_uncalibrated_gyro(Sensor_Data* sd);

void get_uncalibrated_acc(Sensor_Data* sd);

void calibrate_gyro();

void init_sensors(void);

void initialize_all(void);

void send_data(Sensor_Data* sd);

Sensor_Data* create_empty_sensor(bool data);

Sensor_Data* get_sd();

#endif /* SENSOR_MAIN_H_ */