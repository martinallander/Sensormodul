#ifndef SENSOR_DATA
#define SENSOR_DATA

#include <stdbool.h>

#define PACKET_SIZE (sizeof(Sensor_Data))
#define IR_SIZE 256
#define ANGLE_SIZE 12
#define DISTANCE_SIZE 12
#define ACC_SIZE 12

struct Sensor_Data
{
	float acc[3];
	float gyro[3];
	float angle[3];
	float distance[3];
	float ir[64];
	bool has_data;
	bool has_ir;
	bool has_angle;
	bool has_acc;
	bool has_distance;
}; typedef struct Sensor_Data Sensor_Data;

union SPI_packet
{
	Sensor_Data sd;
	unsigned char packet[PACKET_SIZE];
}; typedef union SPI_packet SPI_packet;

union IR_packet
{
	float ir[64];
	unsigned char packet[IR_SIZE];
}; typedef union IR_packet IR_packet;

union Angle_packet
{
	float angle[3];
	unsigned char packet[ANGLE_SIZE];
}; typedef union Angle_packet Angle_packet;

union Distance_packet
{
	float distance[3];
	unsigned char packet[DISTANCE_SIZE];
}; typedef union Distance_packet Distance_packet;

union Acc_packet
{
	float acc[3];
	unsigned char packet[ACC_SIZE];
}; typedef union Acc_packet Acc_packet;

#endif
