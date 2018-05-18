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

/*======================================================
				Gyroscopes registers
========================================================*/
#define gyro_addr			0xD6 // 11010110
#define gyro_ctrl_reg_1		0x20
#define gyro_ctrl_reg_4		0x23
#define gyro_ctrl_reg_5		0x25
#define gyro_status_reg		0x27
#define gyro_FIFO_ctrl_reg	0x2E
#define gyro_FIFO_src_reg	0x2F

#define gyro_x_l			0x28
#define gyro_x_h			0x29
#define gyro_y_l			0x2A
#define gyro_y_h			0x2B
#define gyro_z_l			0x2C
#define gyro_z_h			0x2D
/*=====================================================================
				Gyroscopes setup values
=======================================================================*/
#define gyro_range_250dps		0x00		//Värde som ska skrivas till control register 4 för 250dps
#define gyro_range_500dps		0x10		//värde som ska skrivas till control register 4 för 500dps
#define gyro_ctrl_reg_1_95		0x0F		//default 95 Hz, all axles
#define gyro_ctrl_reg_1_95_z	0x0C		//95 Hz, only z-axle
#define gyro_ctrl_reg_1_190		0x4F		//190 Hz, all axle
#define gyro_ctrl_reg_1_190_z	0x4C		//190 Hz, only z-axle
#define reboot_memory			0x80		//value to be written to ctrl_reg5 to reboot memory content
//#define enable_FIFO_th1 0x40				//value to be written to ctrl_reg5 to enable FIFO
//#define set_FIFO_stream_th1 0x40			//value to be written to FIFO_ctrl_reg to set stream mode with threshold 1
#define block_new_data_250dps	0x80		//value to be written to ctrl_reg4 to block new data until
											// dataregisters are read from at 250dps. obs! ctrl_reg4 handles the dps-range
#define block_new_data_500dps	0x90		//value  to be written to ctrl_reg4 to block new data until
											// dataregisters are read from at 500dps. obs! ctrl_reg4 handles the dps-range
	
//Gyro sensitivities
#define L3GD20_SENSITIVITY_250DPS (0.00875F)      // Roughly 22/256 for fixed point match
#define L3GD20_SENSITIVITY_500DPS (0.0175F)       // Roughly 45/256
#define L3GD20_SENSITIVITY_2000DPS (0.070F)       // Roughly 18/256
#define L3GD20_DPS_TO_RADS (0.017453293F)		  // degress/s to rad/s multiplier


