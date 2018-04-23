#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "Sensor_main.h"

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
		mean_x += sd_l->gyro_x;
		mean_y += sd_l->gyro_y;
		mean_z += sd_l->gyro_z;
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
		mean_x += sd_l->acc_x;
		mean_y += sd_l->acc_y;
		mean_z += sd_l->acc_z;
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
	//init_temp();
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
	
	sd->acc_x = data_x;
	sd->acc_y = data_y;
	sd->acc_z = data_z;
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
	
	sd->acc_x = -(data_x - calibrated_acc_x);
	sd->acc_y = -(data_y - calibrated_acc_y);
	sd->acc_z = -(data_z - calibrated_acc_z);
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
	
	sd->gyro_x = data_x;
	sd->gyro_y = data_y;
	sd->gyro_z = data_z;
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
	sd->gyro_x = data_x - calibrated_gyro_x;
	sd->gyro_y = data_y - calibrated_gyro_y;
	sd->gyro_z = data_z - calibrated_gyro_z;
	return;
}

//Integrerar vinkelhastigheten för att beräkna en vinkel
void get_angle(Sensor_Data* sd)
{
	get_gyro(sd);
	gyro_time += timer_1_get_time();
	
	sd->angle_x += (sd->gyro_x * gyro_time) * 3.0;
	sd->angle_y += (sd->gyro_y * gyro_time) * 3.0;
	sd->angle_z += (sd->gyro_z * gyro_time) * 3.0;
	
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
	sd->distance_x += (sd->acc_x * t_squared)/2.0;
	sd->distance_y += (sd->acc_y * t_squared)/2.0;
	sd->distance_z += (sd->acc_z * t_squared)/2.0;
	
	gyro_time += acc_time;
	acc_time = 0.0;
	timer_1_start();
	return;
}

/******************************************************************
************************** SPI FUNCTIONS **************************
******************************************************************/

void send_sensor_data(Sensor_Data* sd)
{
	unsigned char data = 0;
	SPI_Packet sp;
	sp.sd = *sd;
	data = spi_tranceiver(data);
	if (data == 0xAA)
	{
		led_blink_green(1);
	}
	for (int i = 0; i < PACKET_SIZE; i++)
	{
		spi_tranceiver(sp.packet[i]);
	}
}

/******************************************************************
****************************** MAIN *******************************
******************************************************************/

int main(void)
{
	Sensor_Data* sd = create_empty_sensor(true);
	volatile float watch_x = 0.0;
	volatile float watch_y = 0.0;
	volatile float watch_z = 0.0;
	initialize_all();
	
	while(1)
	{
		//get_distance(sd);
		//watch_x = (sd->distance_x) * 100;
		//watch_y = (sd->distance_y) * 100;
		get_distance(sd);
		get_angle(sd);
		watch_x = sd->distance_x * 100.0;
		watch_y = sd->distance_y * 100.0;
		watch_z = sd->acc_z;
	}
	
	free(sd);
	return 0;
}


/*
sd->acc_x = 1.0;
sd->acc_y = 2.0;
sd->acc_z = 3.0;
sd->gyro_x = 4.0;
sd->gyro_y = 5.0;
sd->gyro_z = 6.0;
sd->angle_x = 7.0;
sd->angle_y = 8.0;
sd->angle_z = 9.0;
//float tof_distance;
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