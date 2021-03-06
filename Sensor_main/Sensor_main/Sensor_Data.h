#ifndef SENSOR_DATA
#define SENSOR_DATA

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdlib.h>
#include <stdbool.h>

#define PACKET_SIZE (sizeof(Sensor_Data))
#define IR_SIZE 256
#define ANGLE_SIZE 12
#define DISTANCE_SIZE 4
//#define DISTANCE_SIZE 12
#define ACC_SIZE 12
#define VELOCITY_SIZE 12
#define GYRO_SIZE 12

struct Sensor_Data
{
	float acc[3];
	float gyro[3];
	float angle[3];
	float distance;
	float velocity[3];
	float ir[64];
	float ir_right[64];
	bool has_data;
	bool has_ir;
	bool has_ir_right;
	bool has_angle;
	bool has_acc;
	bool has_velocity;
	bool has_distance;
	bool has_gyro;
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
	float distance;
	//float distance[3];
	unsigned char packet[DISTANCE_SIZE];
}; typedef union Distance_packet Distance_packet;

union Acc_packet
{
	float acc[3];
	unsigned char packet[ACC_SIZE];
}; typedef union Acc_packet Acc_packet;

union Velocity_packet
{
	float velocity[3];
	unsigned char packet[VELOCITY_SIZE];
}; typedef union Velocity_packet Velocity_packet;

union Gyro_packet
{
	float gyro[3];
	unsigned char packet[GYRO_SIZE];
}; typedef union Gyro_packet Gyro_packet;

Sensor_Data* create_empty_sensor(bool data);

Sensor_Data* get_sd();

#endif
