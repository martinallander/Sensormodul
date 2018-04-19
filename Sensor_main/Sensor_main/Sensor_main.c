#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "timer.h"
//#include "VL53L0X_1.0.2/Api/core/inc/vl53l0x_api.h"
//#include "VL53L0X_1.0.2/Api/platform/inc/vl53l0x_platform.h"
#include "definitions.h"
#include "FIFO_Queue.h"
//#include "SPI_slave.c"
#include "i2c.h"

float calibrated_x = 0.0;
float calibrated_y = 0.0;
float calibrated_z = 0.0;

void led_blinker(uint8_t times)
{
	//_delay_ms(500);
	for (uint8_t i = 0; i < times; i++)
	{ 
		PORTB |= (1 << 0);
		_delay_ms(500);
		PORTB = (0 << 0);
		//_delay_ms(500);
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

float format_gyro(uint8_t low, uint8_t high)
{
	//fattar inte varför de skiftar 4 steg åt höger...
	int16_t merged_data = (int16_t)(low + high*256);
	return (float)merged_data * L3GD20_SENSITIVITY_250DPS;
}

float format_temp(uint8_t low, uint8_t high)
{
	//fattar inte varför de skiftar 4 steg åt höger...
	int16_t merged_data = (int16_t)(low + high*256);
	return (float)merged_data * AMG8833_RESOLUTION;
}

void init_temp(void)
{
	i2c_write_reg(temp_addr, frame_rate, 0x00, 1);
	_delay_ms(10);
}

void init_acc(void)
{
	i2c_write_reg(accel_addr, ctrl_reg_1, ctrl_reg_acc_100, 1);
	_delay_ms(10);
}

void init_gyro(void)
{
	i2c_write_reg(gyro_addr, ctrl_reg_1, ctrl_reg_gyro_1, 1);
	_delay_ms(10);
}

void get_temp(Sensor_Data* sd)
{
	//float temperature[64] = {};
	//uint8_t temp_l = i2c_read_reg(temp_addr, start_pixel, 1);
	//uint8_t temp_h = i2c_read_reg(temp_addr, start_pixel + 1, 1);
	uint8_t temp_l;
	uint8_t temp_h;
	for(int i = 0; i < 64; i++)
	{
		temp_l = i2c_read_reg(temp_addr, start_pixel + 2*i, 1);
		temp_h = i2c_read_reg(temp_addr, start_pixel + 2*i + 1, 1);
		sd->ir[i] = format_temp(temp_l, temp_h);
	}
}

void get_acc(Sensor_Data* sd)
{
	uint8_t x_l = i2c_read_reg(accel_addr, acc_x_l, 1);
	_delay_ms(1);
	uint8_t x_h = i2c_read_reg(accel_addr, acc_x_h, 1);
	float data_x = format_acc(x_l, x_h);
	
	uint8_t y_l = i2c_read_reg(accel_addr, acc_y_l, 1);
	_delay_ms(1);
	uint8_t y_h = i2c_read_reg(accel_addr, acc_y_h, 1);
	float data_y = format_acc(y_l, y_h);
	
	uint8_t z_l = i2c_read_reg(accel_addr, acc_z_l, 1);
	_delay_ms(1);
	uint8_t z_h = i2c_read_reg(accel_addr, acc_z_h, 1);
	float data_z = format_acc(z_l, z_h);
	
	sd->acc_x = data_x;
	sd->acc_y = data_y;
	sd->acc_z = data_z;
}

void get_gyro(Sensor_Data* sd)
{
	uint8_t x_l = i2c_read_reg(gyro_addr, acc_x_l, 1);
	_delay_ms(1);
	uint8_t x_h = i2c_read_reg(gyro_addr, acc_x_h, 1);
	float data_x = format_gyro(x_l, x_h);
		
	uint8_t y_l = i2c_read_reg(gyro_addr, acc_y_l, 1);
	_delay_ms(1);
	uint8_t y_h = i2c_read_reg(gyro_addr, acc_y_h, 1);
	float data_y = format_gyro(y_l, y_h);
		
	uint8_t z_l = i2c_read_reg(gyro_addr, acc_z_l, 1);
	_delay_ms(1);
	uint8_t z_h = i2c_read_reg(gyro_addr, acc_z_h, 1);
	float data_z = format_gyro(z_l, z_h);
	
	sd->gyro_x = data_x - calibrated_x;
	sd->gyro_y = data_y - calibrated_y;
	sd->gyro_z = data_z - calibrated_z;
}

void get_basic_gyro(Sensor_Data* sd)
{
	uint8_t x_l = i2c_read_reg(gyro_addr, acc_x_l, 1);
	_delay_ms(1);
	uint8_t x_h = i2c_read_reg(gyro_addr, acc_x_h, 1);
	float data_x = format_gyro(x_l, x_h);
	
	uint8_t y_l = i2c_read_reg(gyro_addr, acc_y_l, 1);
	_delay_ms(1);
	uint8_t y_h = i2c_read_reg(gyro_addr, acc_y_h, 1);
	float data_y = format_gyro(y_l, y_h);
	
	uint8_t z_l = i2c_read_reg(gyro_addr, acc_z_l, 1);
	_delay_ms(1);
	uint8_t z_h = i2c_read_reg(gyro_addr, acc_z_h, 1);
	float data_z = format_gyro(z_l, z_h);
	
	sd->gyro_x = data_x;
	sd->gyro_y = data_y;
	sd->gyro_z = data_z;
}

void calibrate_gyro()
{
	Sensor_Data* sd_l = create_empty_sensor(true);
	float mean_x = 0.0;
	float mean_y = 0.0;
	float mean_z = 0.0;
	
	for(int i = 0; i < 100; i++)
	{
		get_basic_gyro(sd_l);
		mean_x += sd_l->gyro_x;
		mean_y += sd_l->gyro_y;
		mean_z += sd_l->gyro_z;
	}
	
	calibrated_x = mean_x / 100.0;
	calibrated_y = mean_y / 100.0;
	calibrated_z = mean_z / 100.0;
	free(sd_l);
}

void init_sensors(void)
{
	//init_temp();
	//init_acc();
	init_gyro();
}

void main() {
	volatile float time;
	// set all pins of PORTB as output
	DDRB = (1 << DDB0);
	PORTB = (0 << PORTB0);
	int count = 0;
	int tot_count = 0;
	timer1_init(0);
	// toggle PORTB every 500ms (using 16Mhz clock)
	while(1) 
	{
		int count = TCNT1;
		tot_count = get_overflows() * 65535 + count;
		time = (1.0/F_CPU) * (tot_count + 1);
	}
}
/*
int main(void)
{
	volatile float gyro_x;
	volatile float gyro_y;
	volatile float gyro_z;
	volatile float accel_z;
	Sensor_Data* sd = create_empty_sensor(true);
	DDRB = (1 << DDB0);
	PORTB = (0 << PORTB0);
	i2c_init();
	sei();
	init_sensors();
	calibrate_gyro();
	//VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	//Status = VL53L0X_DataInit();
	while(1)
	{
		//get_temp(sd);
		//get_acc(sd);
		get_gyro(sd);
	}
	free(sd);
	return 0;
}
*/