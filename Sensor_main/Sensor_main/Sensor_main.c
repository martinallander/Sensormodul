#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "Sensor_main.h"
#include <stdbool.h>

/* Kalibreringsvärden som räknas ut initialt för att sedan
subtraheras från sensordatan */
float calibrated_gyro_x = 0.0;
float calibrated_gyro_y = 0.0;
float calibrated_gyro_z = 0.0;

float calibrated_acc_x = 0.0;
float calibrated_acc_y = 0.0;
float calibrated_acc_z = 0.0;

/*Timervärden för att kunna integrera fram vinkel 
 och sträcka från sensordatan */
float gyro_time = 0.0;
float acc_time = 0.0;

bool data_sending = false;

Sensor_Data* sd;

/******************************************************************
********************** INITIALIZER FUNCTIONS **********************
******************************************************************/

//Sätter frame rate
void init_temp(void)
{
	i2c_write_reg(temp_addr, frame_rate, 0x00, 1);
	_delay_ms(10);
	return;
}

//Sätter samplingshastigheten till 100 Hz
void init_acc(void)
{
	i2c_write_reg(accel_addr, ctrl_reg_1, ctrl_reg_acc_100, 1);
	_delay_ms(10);
	return;
}

void init_gyro(void)
{
	i2c_write_reg(gyro_addr, ctrl_reg_1, ctrl_reg_gyro_1, 1);
	_delay_ms(10);
	return;
}


//Gör 100 mätningar och beräknar medelvärdet
void calibrate_gyro()
{
	Sensor_Data* sd_l = create_empty_sensor(true);
	float mean_x = 0.0;
	float mean_y = 0.0;
	float mean_z = 0.0;
	
	for(int i = 0; i < 100; i++)
	{
		get_uncalibrated_gyro(sd_l);
		mean_x += sd_l->gyro[0];
		mean_y += sd_l->gyro[1];
		mean_z += sd_l->gyro[2];
	}
	
	calibrated_gyro_x = mean_x / 100.0;
	calibrated_gyro_y = mean_y / 100.0;
	calibrated_gyro_z = mean_z / 100.0;
	free(sd_l);
	_delay_ms(10);
	return;
}

//Gör 100 mätningar och beräknar medelvärdet
void calibrate_acc()
{
	Sensor_Data* sd_l = create_empty_sensor(true);
	float mean_x = 0.0;
	float mean_y = 0.0;
	float mean_z = 0.0;
	
	for(int i = 0; i < 100; i++)
	{
		get_uncalibrated_acc(sd_l);
		mean_x += sd_l->acc[0];
		mean_y += sd_l->acc[1];
		mean_z += sd_l->acc[2];
	}
	
	calibrated_acc_x = mean_x / 100.0;
	calibrated_acc_y = mean_y / 100.0;
	calibrated_acc_z = mean_z / 100.0;
	free(sd_l);
	_delay_ms(10);
	return;
}

//Initierar och kalibrerar alla sensorer
void init_sensors(void)
{
	init_temp();
	init_acc();
	init_gyro();
	calibrate_gyro();
	calibrate_acc();
	return;
}

//Initierar I2C- och SPI-bussen samt sensorer, LEDs och timern
void initialize_all(void)
{
	DDRB = (1 << DDB0);
	PORTB = (0 << PORTB0);
	i2c_init();
	spi_init();
	sei();
	init_sensors();
	data_direction_init();
	led_blink_red(1);
	led_blink_green(1);
	led_blink_yellow(1);
	timer_1_init(8.0);
}

/******************************************************************
********************** FORMAT DATA FUNCTIONS **********************
******************************************************************/

//Formaterar accelerometerdatan till m/s^2
float format_acc(uint8_t low, uint8_t high)
{
	int16_t merged_data = (int16_t)(low | (high << 8)) >> 4;
	return (float)merged_data * gravity_value * accel_MG_LSB;
}

//Formaterar gyrometerdatan till grader/s
float format_gyro(uint8_t low, uint8_t high)
{
	int16_t merged_data = (int16_t)(low + high*256);
	return (float)merged_data * L3GD20_SENSITIVITY_250DPS;
}

//Formaterar IR-datan till grader celsius
float format_temp(uint8_t low, uint8_t high)
{
	int16_t merged_data = (int16_t)(low + high*256);
	return (float)merged_data * AMG8833_RESOLUTION;
}

/******************************************************************
******************** GET SENSOR DATA FUNCTIONS ********************
******************************************************************/

void get_temp(Sensor_Data* sd)
{
	uint8_t temp_l;
	uint8_t temp_h;
	for(int i = 0; i < 64; i++)
	{
		temp_l = i2c_read_reg(temp_addr, start_pixel + 2*i, 1);
		temp_h = i2c_read_reg(temp_addr, start_pixel + 2*i + 1, 1);
		sd->ir[i] = format_temp(temp_l, temp_h);
	}
	return;
}

//Används enbart till att kalibrera accelerometern
void get_uncalibrated_acc(Sensor_Data* sd)
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
	
	sd->acc[0] = data_x;
	sd->acc[1] = data_y;
	sd->acc[2] = data_z;
	return;
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
	
	sd->acc[0] = -(data_x - calibrated_acc_x);
	sd->acc[1] = -(data_y - calibrated_acc_y);
	sd->acc[2] = -(data_z - calibrated_acc_z);
	return;
}

//Används enbart till att kalibrera gyrometern
void get_uncalibrated_gyro(Sensor_Data* sd)
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
	
	sd->gyro[0] = data_x;
	sd->gyro[1] = data_y;
	sd->gyro[2] = data_z;
	return;
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
	sd->gyro[0] = data_x - calibrated_gyro_x;
	sd->gyro[1] = data_y - calibrated_gyro_y;
	sd->gyro[2] = data_z - calibrated_gyro_z;
	return;
}

//Integrerar vinkelhastigheten för att beräkna en vinkel
void get_angle(Sensor_Data* sd)
{
	get_gyro(sd);
	gyro_time += timer_1_get_time();
	
	sd->angle[0] += (sd->gyro[0] * gyro_time) * 3.0;
	sd->angle[1] += (sd->gyro[1] * gyro_time) * 3.0;
	sd->angle[2] += (sd->gyro[2] * gyro_time) * 3.0;
	
	acc_time += gyro_time;
	gyro_time = 0.0;
	timer_1_start();
	return;
}

//Integrerar accelerationen för att beräkna sträcka
void get_distance(Sensor_Data* sd)
{
	get_acc(sd);
	acc_time += timer_1_get_time();
	
	float t_squared = pow(acc_time, 2);
	sd->distance[0] += 100.0 * (sd->acc[0] * t_squared)/2.0;
	sd->distance[1] += 100.0 * (sd->acc[1] * t_squared)/2.0;
	sd->distance[2] += 100.0 * (sd->acc[2] * t_squared)/2.0;
	
	gyro_time += acc_time;
	acc_time = 0.0;
	timer_1_start();
	return;
}

/******************************************************************
************************** SPI FUNCTIONS **************************
******************************************************************/

void send_data(Sensor_Data* sd)
{
	unsigned char data = 0;
	data = SPDR;
	//led_blink_yellow(data);
	switch(data)
	{
		case IR_DATA_REQUEST :
			if(sd->has_ir)
			{
				spi_tranceiver(DATA_OK);
				spi_tranceiver(IR_SIZE);
				IR_packet ir_packet;
				for(int i = 0; i < 64; i++)
				{
					ir_packet.ir[i] = sd->ir[i];
				}
				for (int i = 0; i < IR_SIZE; i++)
				{
					spi_tranceiver(ir_packet.packet[i]);
				}
			}
			break;
		
		case ANGLE_DATA_REQUEST :
			if(sd->has_angle)
			{
				
				spi_tranceiver(DATA_OK);
				
				spi_tranceiver(ANGLE_SIZE);
				Angle_packet angle_packet;
				
				for(int i = 0; i < 3; i++)
				{
					angle_packet.angle[i] = sd->angle[i];
				}
				for (int i = 0; i < ANGLE_SIZE; i++)
				{
					spi_tranceiver(angle_packet.packet[i]);
				}
				for(int i = 0; i < 3; i++)
				{
					sd->angle[i] = 0;
				}
			}
			break;
		
		case DISTANCE_DATA_REQUEST :
			if(sd->has_distance)
			{
				spi_tranceiver(DATA_OK);
				spi_tranceiver(DISTANCE_SIZE);
				Distance_packet distance_packet;
				
				for(int i = 0; i < 3; i++)
				{
					distance_packet.distance[i] = sd->distance[i];
				}
				for (int i = 0; i < DISTANCE_SIZE; i++)
				{
					spi_tranceiver(distance_packet.packet[i]);
				}
			}
			break;
		
		case ACC_DATA_REQUEST :
			if(sd->has_acc)
			{
				spi_tranceiver(DATA_OK);
				spi_tranceiver(ACC_SIZE);
				Acc_packet acc_packet;
				
				for(int i = 0; i < 3; i++)
				{
					acc_packet.acc[i] = sd->acc[i];
				}
				for (int i = 0; i < ACC_SIZE; i++)
				{
					spi_tranceiver(acc_packet.packet[i]);
				}
			}
			break;
			
		case ALL_DATA_REQUEST :
			if(sd->has_data)
			{
				spi_tranceiver(DATA_OK);
				spi_tranceiver(PACKET_SIZE);
				SPI_packet spi_packet;
				spi_packet.sd = *sd;
				for (int i = 0; i < PACKET_SIZE; i++)
				{
					spi_tranceiver(spi_packet.packet[i]);
				}
			}
			break;
			
		default :
			led_blink_green(1);
			spi_tranceiver(DATA_ERROR);
			break;
	}
	data_sending = false;
	return;
}

/* 
	********* AVBROTT **********
	Kan finnas flera ISR som använder olika avbrottsvektorer.
	SPI_STC_vect: SPI Serial Transfer Complete 
	INT0_vect: External interrupt
	
	Externa avbrott:
	EICRA – External Interrupt Control Register A
	EIFR –External Interrupt Flag Register
	EIMSK – External Interrupt Mask Register
	ISC - Interrupt Sense Control (1,1 -> rising edge)
	
	EIMSK = 1<<INT0;                // Enable INT0
	EIFR = (1 << INT0);				// Clear flag
	MCUCR = 1<<ISC01 | 1<<ISC00;	// Trigger INT0 on rising edge
*/

ISR(SPI_STC_vect)  
{  
	//led_blink_green(1);
	if (!(data_sending))
	{
		data_sending = true;
		send_data(sd);
	}
}

/******************************************************************
****************************** MAIN *******************************
******************************************************************/

int main(void)
{
	sd = create_empty_sensor(true);
	initialize_all();
	_delay_ms(1000);
	while(1) 
	{
		get_temp(sd);
	}
	free(sd);
	return 0;
}


/*
unsigned char data = 0;
for (int i = 0; i < 64; i++)
{
	sd->ir[i] = i;
}
SPI_Packet sp;
sp.sd = *sd;
data = spi_tranceiver(data);

if (data == 0xAA)
{
	led_blink_green(1);
}
//int x = PACKET_SIZE;
for (int i = 0; i < PACKET_SIZE; i++)
{
	spi_tranceiver(sp.packet[i]);
}
*/