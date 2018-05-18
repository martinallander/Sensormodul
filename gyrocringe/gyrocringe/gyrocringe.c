/*
 * gyrocringe.c
 *
 * Created: 5/18/2018 3:16:37 PM
 *  Author: perli397
 */ 

init_gyro()
{
	i2c_write_reg(gyro_addr, gyro_ctrl_reg_4, gyro_range_250dps, 1);//set resolution to 250dps
	_delay_ms(1);
	i2c_write_reg(gyro_addr, gyro_ctrl_reg_5, 0x40, 1); //enable FIFO:n (???)
	_delay_ms(1);
	i2c_write_reg(gyro_addr, gyro_FIFO_ctrl_reg, 0x00, 1); //set FIFO-mode to bypass
	_delay_ms(1);
	i2c_write_reg(gyro_addr, gyro_ctrl_reg_1, 0x00, 1); // reset
	_delay_ms(1);
	i2c_write_reg(gyro_addr, gyro_ctrl_reg_1, 0x0C, 1); // 95 Hz, only z-axle
	_delay_ms(1);
	return;
}

void init_all(void)
{
	i2c_init();
	uart_init(); //glöm inte att ändra i UART-koden
	sei();
	init_gyro();
	return;
}

void calibrate()
{
	int16_t mean_z = 0.0;
}

int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code 
    }
}