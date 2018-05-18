#ifndef GYROCRINGE_H_
#define GYROCRINGE_H_

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
#include "i2c.h"
#include "uart.h"
/*----------------------------------------------------------------------
							data package
----------------------------------------------------------------------*/
float gyro;
float angle; //ska det vara en array?? med [3]??
/************************************************************************/

/*=====================================================================
						Gyroscope functions
=======================================================================*/

void init_gyro(void);

void init_all(void);

void calibrate(void);

void zdata_avaliable(void);

void get_calibration_data(void);

void get_raw_data(void); // returnerar raw_data?

float merge_data(uint8_t low, uint8_t high);

float get_angle(void); //gyro som indata?


#endif