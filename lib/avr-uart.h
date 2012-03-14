#ifndef __AVR_UART__
#define __AVR_UART__

#include <iom16v.h>
#include <macros.h>

void uart_init(void)
{
	CLI();				//disable interrupt
	UCSRA = 0x20;
	UCSRB = 0x98;			//disable while setting baud rate 
	UCSRC = 0x86;
	UBRRL = 47;			//set baud rate lo 
	UBRRH = 0;
	TIMSK = 0x00;			//timer interrupt sources 
	SEI();				//enable interrupt
}

/*
#pragma interrupt_handler uart0_rx_isr:12
void uart0_rx_isr(void)
{
	while (!(UCSRA & (1 << RXC)));
	TxBuf[m] = UDR;
	m++;
	if (m == 32) {
		m = 0;
		n = 1;
	}
}
*/

void uart_put_char(char R_Byte)
{
	while (!(UCSRA & (1 << UDRE)));
	UDR = R_Byte;
}

#endif
