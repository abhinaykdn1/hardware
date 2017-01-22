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
static unsigned char rst_ok=0;
static unsigned char ur=0;

void uart_init(void) 
{
        // baud=7=115.2K with an external 14.7456MHz crystal
        // baud=9=115.2K with an external 18.4320MHz crystal
        unsigned int baud=((((unsigned long)F_CPU / BAUDRATE)-8 ) >> 4);  
#ifdef VAR_88CHIP
        UBRR0H=(unsigned char) (baud >>8);
        UBRR0L=(unsigned char) (baud & 0xFF);
        /* enable tx/rx and no interrupt on tx/rx */
        UCSR0B =  (1<<RXEN0) | (1<<TXEN0);
        /* format: asynchronous, 8data, no parity, 1stop bit */
        UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
#else
        UBRRH=(unsigned char) (baud >>8);
        UBRRL=(unsigned char) (baud & 0xFF);
        /* enable tx/rx and no interrupt on tx/rx */
        UCSRB =  (1<<RXEN) | (1<<TXEN);
        /* format: asynchronous, 8data, no parity, 1stop bit */
        UCSRC = (1<<URSEL)|(3<<UCSZ0);
#endif
}

/* send one character to the rs232 */
void uart_sendchar(char c) 
{
#ifdef VAR_88CHIP
        /* wait for empty transmit buffer */
        while (!(UCSR0A & (1<<UDRE0)));
        UDR0=c;
#else
        /* wait for empty transmit buffer */
        while (!(UCSRA & (1<<UDRE)));
        UDR=c;
#endif
}
/* send string to the rs232 */
void uart_sendstr(char *s) 
{
        while (*s){
                uart_sendchar(*s);
                s++;
        }
}

void uart_sendstr_p(const prog_char *progmem_s)
/* print string from program memory on rs232 */
{
        char c;
        while ((c = pgm_read_byte(progmem_s++))) {
                uart_sendchar(c);
        }

}

/* get a byte from rs232
* this function does a blocking read */
unsigned char uart_getchar_noanim(unsigned char kickwd)  
{
#ifdef VAR_88CHIP
        while(!(UCSR0A & (1<<RXC0))){
                // we can not aford a watchdog timeout 
                // because this is a blocking function
                if (kickwd){
                        wd_kick();
                }
        }
        return(UDR0);
#else
        while(!(UCSRA & (1<<RXC))){
                // we can not aford a watchdog timeout 
                // because this is a blocking function
                if (kickwd){
                        wd_kick();
                }
        }
        return(UDR);
#endif
}

/* get a byte from rs232
* this function does a blocking read */
unsigned char uart_getchar(unsigned char kickwd)  
{
        unsigned char l=1;
#ifdef VAR_88CHIP
        while(!(UCSR0A & (1<<RXC0))){
#else
        while(!(UCSRA & (1<<RXC))){
#endif
                // we can not aford a watchdog timeout 
                // because this is a blocking function
                if (kickwd){
                        wd_kick();
                }
                if (prg_state_get()){
                        // programming is ongoing
                        if (rst_ok){
                                LED_ON;
                        }
                        ur=0;l=1;
                        continue;
                }
                if (l==0){
                        // only every 256th
                        ur+=analog2v(convertanalog(RESETADC)) ; // voltage on reset pin
                        ur=ur/2;
			if (ur<10){ // less than 1.0V, reset active or cable open
                                LED_OFF;
                                rst_ok=0;
                        }else{
                                if (LED_IS_OFF){ // test only once
                                        if(reset_works()){
                                                // reset is connected correctly
                                                // LED full on
                                                LED_ON;
                                                rst_ok=1;
                                        }
                                }
                        }
		}
                l++; // only once in a while, will wrap at 8-bit
        }
#ifdef VAR_88CHIP
        return(UDR0);
#else
        return(UDR);
#endif
}

/* read and discard any data in the receive buffer */
void uart_flushRXbuf(void)  
{
        unsigned char tmp;
#ifdef VAR_88CHIP
        while(UCSR0A & (1<<RXC0)){
                tmp=UDR0;
        }
#else
        while(UCSRA & (1<<RXC)){
                tmp=UDR;
        }
#endif
}

