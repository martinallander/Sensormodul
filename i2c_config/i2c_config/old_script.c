/*
 * i2c_config.c
 *
 * Created: 3/25/2018 10:28:29 AM
 *  Author: alfhu925
 */

/*=====================================================================================
 Bra grejer att ha koll p�:
 * Man kan f� ut v�rdet p� en variabel i realtid genom att:
	- antingen genom att tixa med breakpoint
	- eller h�gerklicka p� variabeln
	- eller en kombination av b�da.....
=======================================================================================*/

#include <math.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdio.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif


#define F_SCL 50UL
#define Prescaler 1
#define TWBR_value ((((F_CPU/F_SCL)/Prescaler)-16)/2)

#define ERROR 1
#define SUCCESS 0

#define I2C_WRITE	0
#define I2C_READ	1

#define accel_addr 0x32
#define test_reg_addr 0x00
#define ctrl_reg_1 0x20
#define acc_x_l_reg 0x28
#define acc_x_h_reg 0x29
#define acc_y_l_reg 0x2A
#define acc_y_h_reg 0x2B
#define acc_z_l_reg 0x2C
#define acc_z_h_reg 0x2D

//volatile unsigned char i2c_addr;

//Initializer of i2c
void i2c_init(void)
{	
	//M�ste vi enable TWIE TWI interrupt ocks�? JA!
	sei();
	TWSR = 0x00; //Set prescaler-bitar to zero.
	TWBR = 0x0C;
	TWCR = (1 << TWEN);
}

uint8_t i2c_start(uint8_t addr) //Takes the slave address and write/read bit. SAD + W/R.
{
	TWCR = 0; //reset I2C control register
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
	
	if((TWSR & 0xF8) != TW_START)
	{
		return ERROR;
	}
	TWDR = addr; //load TWDR with slave address
	TWCR = (1 << TWINT)|(1 << TWEN); //start transmission of address
	
	while(!(TWCR & (1 << TWINT))); //Wait for the transmission to finish
	uint8_t twst = TW_STATUS & 0xF8;
	
	if((twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK)) //check for acknowledge from slave
	{
		return ERROR;
	} 
	return SUCCESS;
}

uint8_t i2c_repeated_start(uint8_t addr)
{
	return i2c_start(addr);
}

void i2c_stop(void)
{
	TWCR = (1 << TWINT)|(1 << TWSTO)|(1 << TWEN);
}

uint8_t i2c_write(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWINT)|(1 << TWEN); //start transmission of data
	while(!(TWCR & (1 << TWINT)));  //wait for the transmission to finish
	
	if((TWSR & 0xF8) != TW_MT_DATA_ACK) //Check for data-transmit-acknowledge 
	{
		return ERROR;
	}
	return SUCCESS;
}

uint8_t i2c_ack_read(void)
{
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWEA);
	while(!(TWCR & (1 << TWINT)));
	return TWDR;
}

uint8_t i2c_nack_read(void)
{
	TWCR = (1 << TWINT)|(1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
	return TWDR;
}

//uint8_t i2c_get_status(void)
//{
//uint8_t status;
//status = (TWSR & 0xF8);
//return status;
//}

//Blink LED "times" number of times
void led_blinker(uint8_t times)
{
	for (uint8_t i = 0; i < times; i++)
	{
		PORTB |= (1<<0);
		_delay_ms(10);
		PORTB = (0<<0);
		_delay_ms(10);
	}
}


//The following functions are specific to each unit on the I2C-bus
//Accelerometer - LSM303DLHC
void i2c_write_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t data)
{
	i2c_start(dev_addr + I2C_WRITE);
	i2c_write(reg_addr);
	i2c_write(data);
	i2c_stop();
}

//Data �r d�r data sparas
short i2c_read_reg(uint8_t dev_addr, uint8_t reg_addr)
{
	i2c_start(dev_addr + I2C_WRITE);
	i2c_write(reg_addr);
	i2c_repeated_start(dev_addr + I2C_READ);
	//os�ker p� om man ska ha och hur data = i2c_nack_read() fungerar...
	uint8_t data = i2c_ack_read();
	i2c_stop();	
	return data;
}

/*
OBS! M�ste skapa en array-variabel f�r att spara undan datan. f�r att sedan tillkalla
funktionen med. Kom ih�g l�ngd p� arrayen. Finns dynamisk variant. MALLOC google �r
din v�n.

KOLLA UPP OM MAN M�STE S�TTA EN VISS BIT I REGISTRET F�R ATT UT�KA DET!!!!
*/
void i2c_mult_read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t length)
{
	i2c_start(dev_addr + I2C_WRITE);
	i2c_write(reg_addr);
	i2c_repeated_start(dev_addr + I2C_READ);
	
	for (uint16_t i = 0; i < (length -1); i++)
	{

		data[i] = i2c_ack_read();
	}
	data[(length - 1)] = i2c_nack_read();
	
	i2c_stop();
}

short shift_data(uint8_t high, uint8_t low)
{
	uint16_t data = 0;
	uint16_t shifted = shifted * pow(2,8);
	shifted + low;
	return (short) shifted;
}

/*
hur man "Castar" till ny datatyp ex: av Oscar!
uint8_t sparad_data_16_B
(float) sparad_data_16_B

*/


	/*
	Beh�ver vi maska bort prescale eller sker det redan i i2c_start?
	TWSR_no_prescale = ()
	==============================================================
	Hur �r ordningen?
	S�tts TWSR f�rst f�r att senare k�ra i2c_start t.ex.?
	eller m�ste i2c_start k�ras f�rst f�r att TWSR 
	Verkar vara s� att TWINT s�tts efter att ett kommando p� I2C-bussen har skickats
	
	Allts� �r fr�gan:
	Hur g�r det hela tillv�ga? Hur hamnar vi i ISR()?
	�r det s� att vi s�tter upp vad som ska h�nda efter/vid varje specifik l�ge f�r
	statusregistret TWSR? har vi inte redan gjort det?
	
	Kanske vi ska ers�tta/s�tta in ovanst�ende funktioner med/i f�ljande "switch-case"?
	
	Blir "statement":en i vaje case vad vi vill ska h�nda EFTER att TWSR �r case? 
	
	================================================================
	
	Jag tror/tolkar det som om att f�ljande avbrottsrutin ers�tter funktionerna ovan!!
	 
	 Eller ska vi ist�llet ha avbrottsrutiner som anv�nder funktionerna ovan
	 genom att bara ha f�r i2c_read_reg/i2c_write_reg?
	*/
	
//void ISR()
//{
	//switch ((TWSR & 0xF8))
	//{
	//case TW_START	:
	///*
	//TROR DETTA SKA VARA MED I i2c_init() ELLER LIKNANDE.
	//DET SKA I ALLA FALL SKICKAS INNAN MAN HAR KOMMIT IN H�R!
//
		//TWCR = 0; //reset I2C control register
		//TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
		//*/
		//break;
	//case TW_MT_SLA_ACK	:
		//break;
	//case TW_MT_SLA_NACK	:
		//break;
	//case TW_MT_DATA_ACK	:
		//break;
	//case TW_MT_DATA_NACK:
		//break;
	//case TW_MR_SLA_ACK	:
		//break;
	//case TW_MR_SLA_NACK	:
		//break;
	//case TW_MR_DATA_ACK	:
		//break;
	//case TW_MR_DATA_NACK:
		//break;
	//
	//}
//};

int main(void)
{
		
	DDRB = (1 << DDB0);
	PORTB = (0 << PORTB0);
	//_delay_ms(500);
    //PORTB = 0x00;
	//kanske beh�ver en array?

	i2c_init();
	volatile uint8_t x_l_data = 0;
	volatile uint8_t x_h_data = 0;
	volatile uint8_t y_l_data = 0;
	volatile uint8_t y_h_data = 0;
	volatile uint8_t z_l_data = 0;
	volatile uint8_t z_h_data = 0;
	//while (1)
	//{
		//accel_i2c_read_reg(test_reg_addr, saved_data);
	//}
	uint8_t set_ctrl_reg_1 = 0b01000111;
	int i = 0;
	i2c_write_reg(accel_addr, ctrl_reg_1, set_ctrl_reg_1);
	while(i < 1000)
	{
		x_l_data = i2c_read_reg(accel_addr, acc_x_l_reg);
		x_h_data = i2c_read_reg(accel_addr, acc_x_h_reg);
		y_l_data = i2c_read_reg(accel_addr, acc_y_l_reg);
		y_h_data = i2c_read_reg(accel_addr, acc_y_h_reg);
		z_l_data = i2c_read_reg(accel_addr, acc_z_l_reg);
		z_h_data = i2c_read_reg(accel_addr, acc_z_h_reg);
		volatile short data_x = shift_data(x_l_data, x_h_data);
		volatile short data_y = shift_data(y_l_data, y_h_data);
		volatile short data_z = shift_data(z_l_data, z_h_data);
		//led_blinker(1);
		i = i + 1;
	}
	//DDRA = final_data;
	//printf("Detta �r v�rdet p� sparad data%d\n", saved_data);	
	
	
	//led_blinker(final_data);
	led_blinker(1);
	//if (final_data == 0x55)
	//{
		//led_blinker(50);
	//}
	//else
	//{
		//led_blinker(1);
	//}
	//return SUCCESS;
	return 0;
}




//uint8_t i2c_transmit(uint8_t addr, uint8_t* data, uint16_t length)
//{
	////Os�ker p� om en startsignal ska skickas innan skrivning. Kan fungera som kontroll
	//// av lyckad �verf�ring eller liknande???
	//if (i2c_start(addr | I2C_WRITE))
	//{
		//return 1;
	//}
	//
	//for (uint16_t i = 0; i < length; i++)
	//{
		//if (i2c_write(data[i])) return 1;
	//}
	//
	//i2c_stop();
	//
	//return 0;
//}
//
//uint8_t i2c_receive(uint8_t addr, uint8_t* data, uint16_t length)
//{
	//if (i2c_start(addr | I2C_READ))
	//{
		//return 1;
	//}
	//
	//for (uint16_t i = 0; i < (length-1); i++)
	//{
		//data[i] = i2c_ack_read();
	//}
	//
	//data[(length-1)] = i2c_nack_read();
	//
	//i2c_stop();
	//
	//return 0;
//}

////kan vara fel att anv�nda dessa. De utkommenterade funk. l�ngst ned kan vara
////l�mpligare. problemet som uppst�r �r "data-pekaren". Antingen g�r om till vanlig
////int. eller anv�nda utkommenterade funktioner typ.
//uint8_t i2c_write_register(uint8_t device_addr, uint8_t register_addr, uint8_t* data, uint16_t length)
//{
	//if (i2c_start(device_addr | 0x00))
	//{
		//return 1;
	//}
	//
	//i2c_write(register_addr);
//
	//for (uint16_t i = 0; i < length; i++)
	//{
		//if (i2c_write(data[i])) 
		//{
			//return 1;
		//}
	//}
	//
	//i2c_stop();
//
	//return 0;
//}
//
///*
//vad ska egentligen "data"-parametern vara? d�r kommer den l�sta datan att hamna.
//men om man tar TWDR som jag tror, s� skriver man ju TWDR till TWDR. Man kanske ska
//skriva till n�n port vid test f�r att senare skriva till SPI kanalen f�r att skicka
//vidare till Raspberry PI:n?
//*/
//uint8_t i2c_read_register(uint8_t device_addr, uint8_t register_addr, uint8_t* data, uint16_t length)
//{
	//if (i2c_start(device_addr))
	//{
		//return 1;
	//}
//
	//i2c_write(register_addr);
//
	//if (i2c_start(device_addr | 0x01)) 
	//{
		//return 1;
	//}
//
	//for (uint16_t i = 0; i < (length-1); i++)
	//{
		//data[i] = i2c_ack_read();
	//}
	//
	//data[(length-1)] = i2c_nack_read();
//
	//i2c_stop();
//
	//return 0;
//}