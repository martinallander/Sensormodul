#include "uart.h"

//Startar Send Mode
void send_mode(void)
{
	UCSR0B = (1<<TXEN0)|(0<<RXEN0);
}

//Initiera UART
void uart_init(void)
{
	UBRR0H = (unsigned char)(BAUDRATE>>8);							//Skifta registret 8 bitar �t h�ger
	UBRR0L = (unsigned char)BAUDRATE;							//S�tt baudrate
	UCSR0C = (0<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01)|(0<<UPM01);			//S�tter 1 stoppbit, samt 8-bitars dataformat
	recieve_mode();
}

//Skicka med UART
void uart_transmit(unsigned char data)
{
	while(!(UCSR0A & (1<<UDRE0)));									//V�ntar p� att "transmit-buffern" ska bli tom genom att kolla UDREn-flaggan
	UDR0 = data;												    //Ladda in datan i registret (UDR = Uart Data Register)
}

//H�mta/Ta emot med UART
unsigned char uart_recieve(void)
{
	while(!(UCSR0A & (1<<RXC0)));									//V�ntar p� att data ska finnas i "receive-buffern" genom att kolla RXCn-flaggan
	return UDR0;													//Returnera data
}
