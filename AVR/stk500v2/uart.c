/* vim: set sw=8 ts=8 si et: */
/*********************************************
* UART interface without interrupt
* Author: Guido Socher, Copyright: GPL 
* Copyright: GPL
**********************************************/
#include "timeout.h"
#include <avr/interrupt.h>
#include <string.h>
#include <avr/io.h>
#include "uart.h"
#include "analog.h"
#include "led.h"
#include "cfg500.h"
#include "spi.h"
#include "prescaler.h"
#include "events.h"
#include "exclock.h"


extern unsigned int count;





void uart_init(void) 
{
	
	//#define BAUD 115200 //uart baudrate
	//#define MYUBRR F_CPU/16/BAUD-1 //7
	
	/*Set baud rate */
	UBRR0H = (unsigned char) (7 >> 8);
	UBRR0L = (unsigned char) (7);
	/* enable tx/rx and no interrupt on tx/rx */
	UCSR0B =  (1<<RXEN0) | (1<<TXEN0);
	/* format: asynchronous, 8data, no parity, 1stop bit */
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);

}




/* send one character to the rs232 */
void uart_sendchar(char c) 
{
	/* wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0=c;
}




/* get a byte from rs232
* this function does a blocking read */
unsigned char uart_getchar(void) {
	unsigned int p = 0;
	
		
	while(!(UCSR0A & (1<<RXC0))) {
	
	wdt_reset();	
		
		
		
		if(prg_state_get()) {
		
			if (p++ > count) { //exit error due (check timeout)
				spi_stop();
				break;
			}
		
		} else {
			
			suspendmode(); //Проверка, программатор в suspend режиме или нет
			IfCallBoot(); //Проверка, нажата ли кнопка бутлоадера
			targetconnect(); //Проверка, подключена ли таргет плата к программатору
			
		}
		
		
		
		
		
	}

	return(UDR0);

}


