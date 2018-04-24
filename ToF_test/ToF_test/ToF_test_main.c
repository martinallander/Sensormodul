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
											GL�M INTE!(?)
		- Sensorn ska kalibreras f�rsta g�ngen p� n�t s�tt.
		
		- n�gon initiering kr�vs ox�
		
		- beh�ver vi s�ga att det �r i2c?
		
		- structen Device m�ste f� r�tt v�rden
=======================================================================================*/

/*=======================================================================================
		- Det som beh�vs g�ras �r att �ndra funktionerna i vl53l0x_platform.c s�
			de passar till AVR.
		
		- Sen anv�nder vi funktioner fr�n api.c(?) (Eller �r det fr�n platform.c)
		 h�r i main f�r att g�ra det vi �nskar. 	
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