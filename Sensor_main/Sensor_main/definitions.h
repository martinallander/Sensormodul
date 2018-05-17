#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/******************************************************************
******************************** I2C ******************************
******************************************************************/
#define F_SCL 50UL
#define Prescaler 1
#define TWBR_value ((((F_CPU/F_SCL)/Prescaler)-16)/2)

#define ERROR 1
#define SUCCESS 0

#define I2C_WRITE 0
#define I2C_READ 1

//Accelerometer registers
#define accel_addr 0x32

#define acc_ctrl_reg_1 0x20
#define acc_x_l 0x28
#define acc_x_h 0x29
#define acc_y_l 0x2A
#define acc_y_h 0x2B
#define acc_z_l 0x2C
#define acc_z_h 0x2D

//accelerometer seetup values
#define ctrl_reg_acc_10 = 0b00100111 //10Hz
#define ctrl_reg_acc_50 0b01000111 //50Hz
#define ctrl_reg_acc_100 0b01010111 //100Hz
#define accel_MG_LSB 0.001f
#define gravity_value 9.821f

//Gyrometer registers
#define gyro_addr 0xD6 // 11010110
#define gyro_ctrl_reg_1 0x20
#define gyro_ctrl_reg_4 0x23
#define gyro_ctrl_reg_5 0x25
#define gyro_status_reg 0x27
#define gyro_FIFO_ctrl_reg 0x2E

//samma som acc. Dock skönare att läsa så här
#define gyro_x_l 0x28
#define gyro_x_h 0x29
#define gyro_y_l 0x2A
#define gyro_y_h 0x2B
#define gyro_z_l 0x2C
#define gyro_z_h 0x2D

//Gyro setup values
#define gyro_range_250dps 0x00		//Värde som ska skrivas till control register 4 för 250dps
#define gyro_range_500dps 0x10		//värde som ska skrivas till control register 4 för 500dps
#define gyro_ctrl_reg_1_95 0x0F		//default 95 Hz, all axles
#define gyro_ctrl_reg_1_95_z 0x0C	//95 Hz, only z-axle
#define gyro_ctrl_reg_1_190 0x4F	//190 Hz, all axle
#define gyro_ctrl_reg_1_190_z 0x4C	//190 Hz, only z-axle
#define reboot_memory 0x80			//value to be written to ctrl_reg5 to reboot memory content
//#define enable_FIFO_th1 0x40		//value to be written to ctrl_reg5 to enable FIFO 
//#define set_FIFO_stream_th1 0x40	//value to be written to FIFO_ctrl_reg to set stream mode with threshold 1
#define block_new_data_250dps 0x80	//value to be written to ctrl_reg4 to block new data until 
									// dataregisters are read from at 250dps. obs! ctrl_reg4 handles the dps-range
#define block_new_data_500dps 0x90	//value  to be written to ctrl_reg4 to block new data until 
									// dataregisters are read from at 500dps. obs! ctrl_reg4 handles the dps-range
//Gyro sensitivities
#define L3GD20_SENSITIVITY_250DPS (0.00875F)      // Roughly 22/256 for fixed point match
#define L3GD20_SENSITIVITY_500DPS (0.0175F)       // Roughly 45/256
#define L3GD20_SENSITIVITY_2000DPS (0.070F)       // Roughly 18/256
#define L3GD20_DPS_TO_RADS (0.017453293F)		  // degress/s to rad/s multiplier

//IR registers
#define temp_addr 0xD2
#define start_pixel 0x80 //Lowbyte för första pixeln (nr 0)
#define end_pixel 0xFE	//Lowbyte för sista pixeln (nr 63)
#define frame_rate 0x02
#define AMG8833_RESOLUTION 0.25f

/******************************************************************
******************************** SPI ******************************
******************************************************************/

#define ALL_DATA_REQUEST 0x01
#define ACC_DATA_REQUEST 0x02
#define ANGLE_DATA_REQUEST 0x03
#define DISTANCE_DATA_REQUEST 0x04
#define IR_DATA_REQUEST 0x05
#define VELOCITY_DATA_REQUEST 0x06
#define GYRO_DATA_REQUEST 0x07

#define SPI_DATA_ERROR 0x10
#define SPI_DATA_OK 0x11
