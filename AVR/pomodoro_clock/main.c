/*
 * main.c
 *
 *  Created on: May 22, 2014
 *      Author: lim
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "ports.h"

#define LED PORT_A_D13
#define PRGM PORT_A_D2
#define LEDCTRL PORT_A_D8

uint8_t j;

void init() {
	SETD(LED);
	SETD(LEDCTRL);
	//SETP(LED);
	CLRD(PRGM);
	SETP(PRGM);
	j = 0;
}

void chkpgm() {
	if (!PINV(PRGM)) {
		void *bl = (void *)0x3c00;
		goto *bl;
	}
}

void loop() {

	for (uint8_t i = 0; i<54*3; i++) {
		chkpgm();
		uint8_t ibyte = (i+j)&63;
		uint8_t bc = 8;
		asm volatile (
				"	cli"							"\n\t"
				"	eor		__tmp_reg__,__tmp_reg__"	"\n\t"
				"	inc		__tmp_reg__"			"\n\t"
				"	ldi		%[bitcnt],8"			"\n\t"
				"btloop:"							"\n\t"
				"	out		%[port],__tmp_reg__"	"\n\t"
				"	rol		%[byte]"				"\n\t"
				"	nop"							"\n\t"
				"	brcs	bit1"					"\n\t"
				"	out		%[port],__zero_reg__"	"\n\t"
				"	nop"							"\n\t"
				"	nop"							"\n\t"
				"	nop"							"\n\t"
				"	rjmp	bit0"					"\n\t"
				"bit1:"								"\n\t"
				"	nop"							"\n\t"
				"	nop"							"\n\t"
				"	nop"							"\n\t"
				"	nop"							"\n\t"
				"	nop"							"\n\t"
				"bit0:"								"\n\t"
				"	out		%[port],__zero_reg__"	"\n\t"
				"	nop"							"\n\t"
				"	nop"							"\n\t"
				"	nop"							"\n\t"
				"	nop"							"\n\t"
				"	nop"							"\n\t"
				"	nop"							"\n\t"
				"	subi	%[bitcnt],1"			"\n\t"
				"	brne	btloop"					"\n\t"
				"	sei"							"\n\t"
				::
				  [bitcnt] "d" (bc),
				  [port] "I" (_SFR_IO_ADDR(PORTB)),
				  [byte] "r" (ibyte)
		);
/*
		uint8_t j = i;

		for (uint8_t k = 0; k<8; k++) {
			if (j & 128) {
				INVP(LED);
				_delay_ms(225);
				INVP(LED);
				_delay_ms(25);
			} else {
				INVP(LED);
				_delay_ms(25);
				INVP(LED);
				_delay_ms(225);
			}
			j = j << 1;
		}
*/
//		_delay_ms(1000);
	}
	_delay_ms(10);
	j++;
//	_delay_ms(1000);
}

int main(void)
{
	init();
	while (1) {
		loop();
	}
	return 0;
}

