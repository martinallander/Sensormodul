#ifndef SENSOR_DATA
#define SENSOR_DATA

#include <stdbool.h>

struct Sensor_Data
{
	short acc_x;
	short acc_y;
	short acc_z;
	short gyro_x;
	short gyro_y;
	short gyro_z;
	short angle_x;
	short angle_y;
	short angle_z;
	//float tof_distance;
	short ir[64];
	bool has_data;
}; typedef struct Sensor_Data Sensor_Data;

/*
struct Angles
{
	float prev_x;
	float prev_y;
	float prev_z;
}; typedef struct Angles Angles;
*/

#define PACKET_SIZE (sizeof(Sensor_Data))

union SPI_Packet
{
	Sensor_Data sd;
	unsigned char packet[PACKET_SIZE];
}; typedef union SPI_Packet SPI_Packet;

#endif
