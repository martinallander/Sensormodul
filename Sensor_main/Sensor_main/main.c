#include "data_transfer.h"
#include "sensor_data.h"

int main(void)
{
	Sensor_Data* current_data = get_sd();
	current_data = create_empty_sensor(true);
	initialize_all();
	while(1)
	{
		get_temp(current_data);
		get_angle(current_data);
	}
	free(current_data);
	return 0;
}