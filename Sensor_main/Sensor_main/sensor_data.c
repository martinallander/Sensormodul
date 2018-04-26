
#include "sensor_data.h"

Sensor_Data* sd;

Sensor_Data* create_empty_sensor(bool data)
{
	Sensor_Data sd;
	Sensor_Data* sd_p = malloc(sizeof(sd));
	sd_p->acc[0] = 0;
	sd_p->acc[1] = 0;
	sd_p->acc[2] = 0;
	sd_p->gyro[0] = 0;
	sd_p->gyro[1] = 0;
	sd_p->gyro[2] = 0;
	sd_p->angle[0] = 0;
	sd_p->angle[1] = 0;
	sd_p->angle[2] = 0;
	sd_p->distance = 0;
	/*
	sd_p->distance[0] = 0;
	sd_p->distance[1] = 0;
	sd_p->distance[2] = 0;
	*/
	sd_p->has_data = data;
	sd_p->has_ir = data;
	sd_p->has_angle = data;
	sd_p->has_acc = data;
	sd_p->has_distance = data;
	return sd_p;
}

Sensor_Data* get_sd()
{
	return sd;
}