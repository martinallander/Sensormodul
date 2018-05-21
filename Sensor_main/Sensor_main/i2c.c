#include "i2c.h"

/************************************************************************/
/*****************Globala variabler för hantering av I2C*****************/
/************************************************************************/

volatile bool i2c_done = 1;
volatile bool write_to_slave = 1;
volatile uint8_t trans_data;
volatile uint8_t rec_data;
volatile uint8_t device_addr;
volatile uint8_t register_addr;
volatile int n_o_writes = 0;
volatile int n_o_reads = 0;


//Initializer of i2c
void i2c_init(void)
{	
	TWSR = 0x00; //Set prescaler-bitar to zero.
	TWBR = 0x92; //från källe-man
	TWCR = (1 << TWEN)|(1 << TWIE);
}

//skickar ett startvillkor
void i2c_start(void)
{
	TWCR = (1 << TWINT)|(1 << TWSTA)|(0 << TWSTO)|(1 << TWEN)|(1 << TWIE);
	i2c_done = 0;
}

//skicka ett stopvillkor
void i2c_stop(void)
{
	TWCR = (1 << TWINT)|(0 << TWSTA)|(1 << TWSTO)|(1 << TWEN)|(1 << TWIE);
	i2c_done = 1;
}

//Lägger data på I2C-bussen
void i2c_send_data(uint8_t data)
{
		TWDR = data;
		TWCR = (1 << TWINT)|(0 << TWSTA)|(0 << TWSTO)|(1 << TWEN)|(1 << TWIE);
}

//avbrottsrutin för hantering av I2C-villkor
ISR(TWI_vect)
{
	uint8_t status = (TWSR & 0xF8);
	switch (status)
	{
		case TW_START:						 //0x08
			i2c_send_data(device_addr + I2C_WRITE);
			break;
		case TW_REP_START:					 //0x10
			i2c_send_data(device_addr + I2C_READ);
			break;
		case TW_MT_SLA_ACK:					 //0x18
			i2c_send_data(register_addr);		 //load the register we want to handle
			break;
		case TW_MT_SLA_NACK:				 //0x20
			i2c_stop();	
			break;
		case TW_MT_DATA_ACK:				 //0x28
			if(write_to_slave)
			{
				if(n_o_writes == 0)
				{
					i2c_stop();
					break;
				}
				else
				{
					i2c_send_data(trans_data);
				}
				n_o_writes = n_o_writes - 1; 
			}
			else
			{
				i2c_start(); //repeated start
			}
		
			break;
		case TW_MR_SLA_ACK: //0x40
			TWCR = (1 << TWINT)|(0 << TWSTA)|(0 << TWSTO)|(0 << TWEA)|(1 << TWEN)|(1 << TWIE);
			break;
		case TW_MR_DATA_ACK: //0x50
			rec_data = TWDR;
			i2c_stop();
			break;
		case TW_MR_DATA_NACK: //0x58
			rec_data = TWDR;
			i2c_stop();
			break;
	}
}

//Skriv till ett register hos en enhet på I2C-bussen
void i2c_write_reg(uint8_t device_address, uint8_t reg_addr, uint8_t data, int n)
{
	while(!i2c_done){};
	n_o_writes = n;
	register_addr = reg_addr;
	device_addr = device_address;
	trans_data = data;
	write_to_slave = 1;
	i2c_start();
}

//Läs från ett register hos en enhet på I2C-bussen
uint8_t i2c_read_reg(uint8_t device_address, uint8_t reg_addr, int n)
{
	while(!i2c_done){};
	n_o_reads = n;
	register_addr = reg_addr;
	device_addr = device_address;
	write_to_slave = 0;
	i2c_start();
	while(!i2c_done){};
	return rec_data;
}
