#include "data_transfer.h"
#include <stdbool.h>

Sensor_Data* current_data;

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

/*Variabler för avståndsmätaren*/

volatile float distance_value;
volatile uint16_t digital_data;

bool data_sending = false;

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
	i2c_write_reg(accel_addr, acc_ctrl_reg_1, ctrl_reg_acc_100, 1);
	_delay_ms(10);
	return;
}

void init_gyro(void)
{
	//i2c_write_reg(gyro_addr, gyro_ctrl_reg_1, 0x00, 1); //reset
	i2c_write_reg(gyro_addr, gyro_ctrl_reg_1, gyro_ctrl_reg_1_value, 1);
	_delay_ms(10);
	i2c_write_reg(gyro_addr, gyro_ctrl_reg_4, gyro_range_250dps, 1);
	_delay_ms(10);
	return;
}

void init_distance(void)
{
	//sätt ADATE för Free Running Mode
	ADCSRA = (1<<ADEN) | (1<<ADIE) | (1<<ADPS1) | (1<<ADPS0);
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
 	init_distance();
 	init_acc();
 	init_gyro();
 	calibrate_gyro();
 	calibrate_acc();
 	return;
}

//Initierar I2C- och SPI-bussen samt sensorer, LEDs och timern
void initialize_all(void)
{
	i2c_init();
	spi_init();
	sei();
	init_sensors();
	data_direction_init();
	led_blink_yellow(1);
	led_blink_green(1);
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
	//int16_t merged_data = (int16_t)(low + high*256);
	int16_t merged_data = (int16_t)(low | (high << 8));
	return (float)merged_data * L3GD20_SENSITIVITY_250DPS;
}

//Formaterar IR-datan till grader celsius
float format_temp(uint8_t low, uint8_t high)
{
	int16_t merged_data = (int16_t)(low + high*256);
	return (float)merged_data * AMG8833_RESOLUTION;
}

float format_distance(uint16_t unformated_data)
{
	//kalibrering v.1.0
	//float formated_data = (float)((18265)*(pow(unformated_data,-1.226)));
	//Kalibrering v.1.1
	//float formated_data = (float)((10879)*(pow(unformated_data,-1.122)));
	//kalibrering v.1.2 avrundat
	//float formated_data = (float)((10991)*(pow(unformated_data,-1.124)));
	//kalibrering v. 1.3
	float formated_data = (float)((7*pow(10,-9))*(pow(unformated_data,4)) + 
									(-1*pow(10,-5)) * (pow(unformated_data,3)) +
									 (0.0056)*(pow(unformated_data,2)) +
									  (-1.4198)*unformated_data +
									   158.22);
	return formated_data;
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
	
	for(int i = 0; i < 3; i++)
	{
		if(sd->acc[i] < 0.01)
		{
			sd->acc[i] = 0.0;
		}
	}
	return;
}


//Integrerar accelerationen för att beräkna hastighet
void get_velocity(Sensor_Data* sd)
{
	get_acc(sd);
	acc_time += timer_1_get_time();
	sd->velocity[0] += (sd->acc[0] * acc_time);
	sd->velocity[1] += (sd->acc[1] * acc_time);
	sd->velocity[2] += (sd->acc[2] * acc_time);
	
	gyro_time += acc_time;
	acc_time = 0.0;
	timer_1_start();
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
	uint8_t x_l = i2c_read_reg(gyro_addr, gyro_x_l, 1);
	_delay_ms(1);
	uint8_t x_h = i2c_read_reg(gyro_addr, gyro_x_h, 1);
	float data_x = format_gyro(x_l, x_h);
		
	uint8_t y_l = i2c_read_reg(gyro_addr, gyro_y_l, 1);
	_delay_ms(1);
	uint8_t y_h = i2c_read_reg(gyro_addr, gyro_y_h, 1);
	float data_y = format_gyro(y_l, y_h);
		
	uint8_t z_l = i2c_read_reg(gyro_addr, gyro_z_l, 1);
	_delay_ms(1);
	uint8_t z_h = i2c_read_reg(gyro_addr, gyro_z_h, 1);
	float data_z = format_gyro(z_l, z_h);
	sd->gyro[0] = data_x - calibrated_gyro_x;
	sd->gyro[1] = data_y - calibrated_gyro_y;
	sd->gyro[2] = data_z - calibrated_gyro_z;
	
	for(int i = 0; i < 3; i++)
	{
		if(abs(sd->gyro[i]) < 1.0)
		{
			sd->gyro[i] = 0.0;
		}
	}
	return;
}

//Integrerar vinkelhastigheten för att beräkna en vinkel
void get_angle(Sensor_Data* sd)
{
	get_gyro(sd);
	gyro_time += timer_1_get_time();
	
	sd->angle[0] += (sd->gyro[0] * gyro_time);
	sd->angle[1] += (sd->gyro[1] * gyro_time);
	sd->angle[2] += (sd->gyro[2] * gyro_time);
	
	acc_time += gyro_time;
	gyro_time = 0.0;
	timer_1_start();
	return;
}

/*
void get_angle(Sensor_Data* sd)
{
	get_gyro(sd);
	sd->angle[0] += sd->gyro[0] / 95.0;
	sd->angle[1] += sd->gyro[1] / 95.0;
	sd->angle[2] += sd->gyro[2] / 95.0;
	return;
}
*/

//void measure_distance(void)
//{
	//ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADIE);
	//return;
//}

void get_distance(Sensor_Data* sd)
{
	//measure_distance();
	ADCSRA |= /*(1 << ADEN) |*/ (1 << ADSC) /*| (1 << ADIE)*/;
	//_delay_ms(5);
	while(ADCSRA & (1 << ADIF)){};
	//distance_value = format_distance(digital_data);
	sd->distance = distance_value;
	//sd->distance = 13.37;
	return;
}

//Integrerar accelerationen för att beräkna sträcka
/*
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
*/

/******************************************************************
************************** SPI FUNCTIONS **************************
******************************************************************/

void send_data(Sensor_Data* sd)
{
	unsigned char data = 0;
	data = SPDR;
	switch(data)
	{
		case IR_DATA_REQUEST :
			if(sd->has_ir)
			{
				spi_tranceiver(SPI_DATA_OK);
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
				
				spi_tranceiver(SPI_DATA_OK);
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
			
		case VELOCITY_DATA_REQUEST :
			if(sd->has_velocity)
			{
			
				spi_tranceiver(SPI_DATA_OK);
				Velocity_packet velocity_packet;
			
				for(int i = 0; i < 3; i++)
				{
					velocity_packet.velocity[i] = sd->velocity[i];
				}
			
				for (int i = 0; i < VELOCITY_SIZE; i++)
				{
					spi_tranceiver(velocity_packet.packet[i]);
				}
			
				for(int i = 0; i < 3; i++)
				{
					sd->velocity[i] = 0;
				}
			}
			break;
		
		case DISTANCE_DATA_REQUEST :
			if(sd->has_distance)
			{
				spi_tranceiver(SPI_DATA_OK);
				Distance_packet distance_packet;
				
				distance_packet.distance = sd->distance;
				
				/*
				for(int i = 0; i < 3; i++)
				{
					distance_packet.distance[i] = sd->distance[i];
				}
				*/
				
				for (int i = 0; i < DISTANCE_SIZE; i++)
				{
					spi_tranceiver(distance_packet.packet[i]);
				}
			}
			break;
		
		case ACC_DATA_REQUEST :
			if(sd->has_acc)
			{
				spi_tranceiver(SPI_DATA_OK);
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
			
		case GYRO_DATA_REQUEST :
			if(sd->has_acc)
			{
				spi_tranceiver(SPI_DATA_OK);
				Gyro_packet gyro_packet;
			
				for(int i = 0; i < 3; i++)
				{
					gyro_packet.gyro[i] = sd->gyro[i];
				}
				for (int i = 0; i < GYRO_SIZE; i++)
				{
					spi_tranceiver(gyro_packet.packet[i]);
				}
			}
			break;
			
		case ALL_DATA_REQUEST :
			if(sd->has_data)
			{
				spi_tranceiver(SPI_DATA_OK);
				SPI_packet spi_packet;
				spi_packet.sd = *sd;
				for (int i = 0; i < PACKET_SIZE; i++)
				{
					spi_tranceiver(spi_packet.packet[i]);
				}
			}
			break;
		/*default :
			spi_tranceiver(SPI_DATA_ERROR);
			led_blink_yellow(1);
			break;*/
	}
}

/******************************************************************
****************************** AVBROTT ****************************
******************************************************************/

/* 
	Kan finnas flera ISR som använder olika avbrottsvektorer.
	SPI_STC_vect: SPI Serial Transfer Complete 
	INT0_vect: External interrupt
	
	Externa avbrott:
	EICRA – External Interrupt Control Register A
	EIFR – External Interrupt Flag Register
	EIMSK – External Interrupt Mask Register
	ISC - Interrupt Sense Control (1,1 -> rising edge)
	
	EIMSK = 1<<INT0;                // Enable INT0
	EIFR = (1 << INT0);				// Clear flag
	MCUCR = 1<<ISC01 | 1<<ISC00;	// Trigger INT0 on rising edge
*/

ISR(SPI_STC_vect)  
{  
	if (!(data_sending))
	{
		data_sending = true;
		send_data(current_data);
		data_sending = false;
	}
}

ISR(ADC_vect)
{
	digital_data = (uint16_t)(ADCL | (ADCH << 8));
	/*********************************************************
						Vid kalibrering:
	*********************************************************/
	//Write_data_to_LCD(digital_data); 
	/*------------------------------------------------------*/
	//while(ADCSRA & (1 << ADIF))
	distance_value = format_distance(digital_data);
	//Write_data_to_LCD(distance); 
	//_delay_ms(1);
	//ADCSRA = (0 << ADEN) | (0 << ADIE);
}

int main(void)
{
	//_delay_ms(5000);							//Väntar på att roboten ska stå upp
	initialize_all();
	current_data = create_empty_sensor(true);
	led_blink_red(1);
	volatile float watch = 0.0;
	timer_1_start();
	while(1) 
	{
	//	get_acc(current_data);
	//	get_velocity(current_data);
	//	get_angle(current_data);
		get_gyro(current_data);
		watch = current_data->gyro[0];
		//get_distance(current_data);
	//	watch = current_data->distance;
		//get_temp(current_data);
		
	}
	free(current_data);
	return 0;
}
