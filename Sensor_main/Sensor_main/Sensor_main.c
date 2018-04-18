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
#include "definitions.h"
#include "FIFO_Queue.h"

float accel_MG_LSB = 0.001F;
float gravity_value = 9.821F;

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
	//fattar inte varf�r de skiftar 4 steg �t h�ger...
	int16_t merged_data = (int16_t)(low | (high << 8)) >> 4;// kan beh�va s�tta int16_t p� low och high
	//_delay_ms(10);

	float formated_data = (float)merged_data * gravity_value * accel_MG_LSB;
	return formated_data;
}

float format_temp(uint8_t low, uint8_t high)
{
	//fattar inte varf�r de skiftar 4 steg �t h�ger...
	int16_t merged_data = (int16_t)(low + high*256);// kan beh�va s�tta int16_t p� low och high
	return (float)merged_data*0.25;
}

int main(void)
{
	volatile uint8_t x_l_value;
	volatile uint8_t x_h_value;
	volatile uint8_t y_l_value; 
	volatile uint8_t y_h_value;
	volatile uint8_t z_l_value;
	volatile uint8_t z_h_value;
	
	volatile uint8_t temp_l;
	volatile uint8_t temp_h;
	
	volatile uint8_t ctrl_reg_data;
	volatile float data_x;
	volatile float data_y;
	volatile float data_z;
	
	volatile float temp_data;
	volatile float temperature[64] = {};
		
	volatile float data_1;
	volatile float data_2;
	
	DDRB = (1 << DDB0);
	PORTB = (0 << PORTB0);
	i2c_init();
	sei();
	/*-----------------------------------------------------------------
	We maybe need to look into CTRL_REG4_A(0x23) to adjust the sensitivity
	CTRL_REG2_A(0x21) configurate a HP-filter 
	-----------------------------------------------------------------*/
	//i2c_write_reg(ctrl_reg_1, set_ctrl_reg_1_100, 1);
	//led_blinker(1);
	i2c_write_reg(temp_addr, set_frame_rate, 0x00, 1);
	//ctrl_reg_data = i2c_read_reg(ctrl_reg_1);
	//Sensor_Data sd = create_empty_sensor(true);
	while(1)
	{
		for(int i = 0; i < 64; i++)
		{
			temp_l = i2c_read_reg(temp_addr, start_pixel + 2*i);
			//_delay_ms(100);
			temp_h = i2c_read_reg(temp_addr, start_pixel + 2*i + 1);
			temperature[i] = format_temp(temp_l, temp_h);
			//sd.ir[i] = format_temp(temp_l, temp_h);
		}
		
		//led_blinker(1);
 		data_1 = temperature[28];
		data_2 = temperature[27];
		//temp_l = i2c_read_reg(start_pixel);
		//_delay_ms(1);
		//temp_h = i2c_read_reg(start_pixel + 1);
		//temp_data = format_temp(temp_l, temp_h);
	
		/*
		x_l_value = i2c_read_reg(acc_x_l_reg);
		_delay_ms(10);
		x_h_value = i2c_read_reg(acc_x_h_reg);
		//_delay_ms(1000);
		data_x = data_formater(x_l_value,x_h_value);
		
		y_l_value = i2c_read_reg(acc_y_l_reg);
		_delay_ms(10);
		y_h_value = i2c_read_reg(acc_y_h_reg);
		data_y = data_formater(y_l_value,y_h_value);
		
		z_l_value = i2c_read_reg(acc_z_l_reg);
		_delay_ms(10);
		z_h_value = i2c_read_reg(acc_z_h_reg);
		data_z = data_formater(z_l_value,z_h_value);
		_delay_ms(10);
		*/
	}
	return 0;
}