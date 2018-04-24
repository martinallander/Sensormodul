/*
 * ToF_test.c
 *
 * Created: 4/20/2018 10:26:52 AM
 *  Author: perli397
 */ 

#include "inc/vl53l0x_platform.h"
#include <avr/io.h>

#define VL53L0X_I2C_ADDR  0x29

/*=======================================================================================
											GLÖM INTE!(?)
		- Sensorn ska kalibreras första gången på nåt sätt.
		
		- någon initiering krävs oxå
		
		- behöver vi säga att det är i2c?
		
		- structen Device måste få rätt värden
=======================================================================================*/

/*=======================================================================================
		- Det som behövs göras är att ändra funktionerna i vl53l0x_platform.c så
			de passar till AVR.
		
		- Sen använder vi funktioner från api.c(?) (Eller är det från platform.c)
		 här i main för att göra det vi önskar. 	
=======================================================================================*/

VL53L0X_Dev_t Our_Dev;

Our_Dev.I2cDevAddr = VL53L0X_I2C_ADDR;


//PALDevDataSet(our_VL53L0X, I2cDevAddr, 0x52)
//PALDevDataSet(xd, I2cDevAddr, 0x52);
//VL53L0X_DEV our_VL53L0X;

// our_VL53L0X.I2cDevAddr = 0x52;
// our_VL53L0X->comms_type = VL53L0X_COMMS_I2C;
// our_VL53L0X->comms_speed_khz = 400; //[kHz]



int main(void)
{
	return 0;
}