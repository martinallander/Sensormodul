#ifndef SENSOR_DATA
#define SENSOR_DATA

#include <stdbool.h>

struct Sensor_Data
{
	float acc_x;
	float acc_y;
	float acc_z;
	float gyro_x;
	float gyro_y;
	float gyro_z;
	float angle_x;
	float angle_y;
	float angle_z;
	float tof_distance;
	float ir[64];
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
	unsigned char packet[sizeof(Sensor_Data)];
}; typedef union SPI_Packet SPI_Packet;

#endif
