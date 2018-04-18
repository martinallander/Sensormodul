/*
 * Sensor_main.c
 *
 * Created: 4/12/2018 11:23:35 AM
 *  Author: maral665
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
//#include "VL53L0X_1.0.2/Api/core/inc/vl53l0x_api.h"
//#include "VL53L0X_1.0.2/Api/platform/inc/vl53l0x_platform.h"
#include "definitions.h"
#include "FIFO_Queue.h"

void led_blinker(uint8_t times)
{
	_delay_ms(500);
	for (uint8_t i = 0; i < times; i++)
	{ 
		PORTB |= (1<<0);
		_delay_ms(500);
		PORTB = (0<<0);
		_delay_ms(500);
	}
}

float format_acc(uint8_t low, uint8_t high)
{
	//fattar inte varför de skiftar 4 steg åt höger...
	int16_t merged_data = (int16_t)(low | (high << 8)) >> 4;// kan behöva sätta int16_t på low och high
	//_delay_ms(10);

	float formated_data = (float)merged_data * gravity_value * accel_MG_LSB;
	return formated_data;
}

float format_temp(uint8_t low, uint8_t high)
{
	//fattar inte varför de skiftar 4 steg åt höger...
	int16_t merged_data = (int16_t)(low + high*256);// kan behöva sätta int16_t på low och high
	return (float)merged_data*0.25;
}

void init_ir(void)
{
	i2c_write_reg(temp_addr, set_frame_rate, 0x00, 1);
}

void init_acc(void)
{
	i2c_write_reg(accel_addr, ctrl_reg_1, set_ctrl_reg_1_100, 1);
}

float* get_ir(void)
{
	float temperature[64] = {};
	for(int i = 0; i < 64; i++)
	{
		uint8_t temp_l = i2c_read_reg(temp_addr, start_pixel + 2*i);
		uint8_t temp_h = i2c_read_reg(temp_addr, start_pixel + 2*i + 1);
		temperature[i] = format_temp(temp_l, temp_h);
	}
	return temperature;
}

float* get_acc(void)
{
	float acc[3] = {};
	uint8_t x_l = i2c_read_reg(accel_addr, acc_x_l);
	uint8_t x_h = i2c_read_reg(accel_addr, acc_x_h);
	float data_x = format_acc(x_l,x_h);
		
	uint8_t y_l = i2c_read_reg(accel_addr, acc_y_l);
	uint8_t y_h = i2c_read_reg(accel_addr, acc_y_h);
	float data_y = format_acc(y_l,y_h);
		
	uint8_t z_l = i2c_read_reg(accel_addr, acc_z_l);
	uint8_t z_h = i2c_read_reg(accel_addr, acc_z_h);
	float data_z = format_acc(z_l,z_h);
	
	acc[0] = data_x;
	acc[1] = data_y;
	acc[2] = data_z;
	return acc;
}

int main(void)
{
	DDRB = (1 << DDB0);
	PORTB = (0 << PORTB0);
	i2c_init();
	sei();
	//VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	//Status = VL53L0X_DataInit();
	//
	//i2c_write_reg(ctrl_reg_1, set_ctrl_reg_1_100, 1);

	while(1)
	{

	}
	return 0;
}