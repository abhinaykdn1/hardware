#define F_CPU 8000000L
#define baudrate 19200L
#include <avr/io.h>
#include <avr/interrupt.h>
#include "macro.h"
#include "usart.h"

#define CMD_BUF 4

uint8_t cmd_buf_p;
char cmd_buf[CMD_BUF];

void clearcmdbuf()
{
	cmd_buf_p = 0;
	uint8_t i;
	for (i = 0; i<CMD_BUF; i++) {
		cmd_buf[i] = 0;
	}
}

void executecmd()
{
	if (cmd_buf[0]) {
		switch (cmd_buf[0]) {
			case 'I': {
				char b = 0;
				switch (cmd_buf[1]) {
					case 'A': {
						b = PINA;
						break;
					}
					case 'B': {
						b = PINB;
						break;
					}
					case 'C': {
						b = PINC;
						break;
					}
					case 'D': {
						b = PIND;
						break;
					}
					case 'E': {
						b = DDRA;
						break;
					}
					case 'F': {
						b = DDRB;
						break;
					}
					case 'G': {
						b = DDRC;
						break;
					}
					case 'H': {
						b = DDRD;
						break;
					}
					case 'I': {
						b = PORTA;
						break;
					}
					case 'J': {
						b = PORTB;
						break;
					}
					case 'K': {
						b = PORTC;
						break;
					}
					case 'L': {
						b = PORTD;
						break;
					}
				}
				putchr(chartohex(b>>4));
				putchr(chartohex(b));
				printstr("\n\r");
				break;
			}
			case 'O': {
				char b = hextochar(cmd_buf[3])+(hextochar(cmd_buf[2])<<4);
				switch (cmd_buf[1]) {
					case 'A': {
						PORTA = b;
						break;
					}
					case 'B': {
						PORTB = b;
						break;
					}
					case 'C': {
						PORTC = b;
						break;
					}
					case 'D': {
						PORTD = b;
						break;
					}
					case 'E': {
						DDRA = b;
						break;
					}
					case 'F': {
						DDRB = b;
						break;
					}
					case 'G': {
						DDRC = b;
						break;
					}
					case 'H': {
						DDRD = b;
						break;
					}
				}
				putchr(chartohex(b>>4));
				putchr(chartohex(b));
				printstr("\n\r");
				break;
			}

			default: {
				printstr("ERR:UC\n\r");
				break;
			}
		}
	}
}

void addtocmd(char c)
{
	if ((c == '\n') || (c == '\r')) {
		if (cmd_buf_p>0) {
			executecmd();
		}
		clearcmdbuf();
	} else {
		if (cmd_buf_p<CMD_BUF) {
			char ch = 0;
			if ((c>='a') & (c<='z')) {
				ch = c - 0x20;
			} else if (((c>='0') & (c<='9')) | ((c>='A') & (c<='Z'))) {
				ch = c;
			}
			if (ch != 0) {
				cmd_buf[cmd_buf_p] = ch;
				cmd_buf_p++;
				//putchr(ch);
			}
		}
	}
}

int main(void)
{
	initUSART();
	clearcmdbuf();

	printstr("SD0.01\n\r");
	DDRD |= bv(5);

	while (1)
	{
		cli();
		ibi(PORTD, 5);
		sei();
		if (chrready()) {
			char c = getchr();
			putchr(c);
			addtocmd(c);
		}
	}
}





