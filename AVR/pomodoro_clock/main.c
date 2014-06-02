/*
 * main.c
 *
 *  Created on: May 22, 2014
 *      Author: lim
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "ports.h"
#include "macro.h"
#include "usart.h"

#define LED PORT_A_D13
#define PRGM PORT_A_D2
#define LEDCTRL PORT_A_D8

const uint16_t PROGMEM numfnt [] = {
		0b0111010101110000,
		0b0000010011111000,
		0b1100111110010000,
		0b1000111101011000,
		0b0011101011111000,
		0b1011111101001000,
		0b0111011101000000,
		0b1100111000011000,
		0b0101011101010000,
		0b0001011101110000,
		0b1111011011110000,
		0b1111111101010000,
		0b0111010101010000,
		0b1111110101110000,
		0b1111111110001000,
		0b1111111000001000

//		0b1111110111111000,
//		0b0000000011111000,
//		0b1110111110111000,
//		0b1000111111111000,
//		0b0011101011111000,
//		0b1011111111101000,
//		0b1111111111101000,
//		0b0000110011111000,
//		0b1111111111111000,
//		0b1011111111111000,
//		0b1111011011110000,
//		0b1111111101010000,
//		0b0111010101010000,
//		0b1111110101110000,
//		0b1111111110001000,
//		0b1111111000001000
};

uint8_t j = 0;
uint16_t k = 0;
uint8_t pixels[54*3];
uint8_t needrefresh = 1;
uint8_t needshow = 1;

uint8_t hours = 0;
uint8_t minutes = 0;
uint8_t seconds = 0;
uint8_t t_minutes = 0;
uint8_t t_seconds = 0;
uint8_t t_workmin = 45;
uint8_t t_restmin = 5;

uint8_t defred = 16;
uint8_t defgreen = 4;
uint8_t defblue = 0;

uint8_t mode = 0; //0 - hh:mm, 1 - mm:ss, 2 - ptimer wrk, 3 - ptimer rest

void clrscr() {
	for (uint8_t i = 0; i<54*3; i++) {
		pixels[i] = 0;
	}
}

void setnum(uint8_t pos, uint8_t num) {
	uint8_t saddr = pos*13;
	saddr += (pos>1)?2:0;
	uint16_t symbit = pgm_read_word(&numfnt[num]);
	uint8_t i = 13;
	while (i) {
		uint8_t pxr = defred;
		uint8_t pxg = defgreen;
		uint8_t pxb = defblue;
		uint8_t msk = (symbit&0x8000)?255:0;
		pixels[saddr*3+0] = pxg&msk;
		pixels[saddr*3+1] = pxr&msk;
		pixels[saddr*3+2] = pxb&msk;
		symbit = symbit<<1;
		saddr++;
		i--;
	}
}

void setpt(uint8_t pos, uint8_t num) {
	uint8_t saddr = 2*13+(pos&1);
	uint8_t pxr = defred;
	uint8_t pxg = defgreen;
	uint8_t pxb = defblue;
	uint8_t msk = (num&1)?255:0;
	pixels[saddr*3+0] = pxg&msk;
	pixels[saddr*3+1] = pxr&msk;
	pixels[saddr*3+2] = pxb&msk;
}

void refresh() {
	cli();
	for (uint8_t i = 0; i<54*3; i++) {
		uint8_t ibyte = pixels[i];
		uint8_t bc = 8;
		asm volatile (
//				"	cli"							"\n\t"
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
//				"	sei"							"\n\t"
				::
				  [bitcnt] "d" (bc),
				  [port] "I" (_SFR_IO_ADDR(PORTB)),
				  [byte] "r" (ibyte)
		);
	}
	sei();
}

void startwork() {
	t_minutes = t_workmin;
	t_seconds = 0;
	mode = 2;
	printstr("Start work\n\r\0");
}

void startrest() {
	t_minutes = t_restmin;
	t_seconds = 0;
	mode = 3;
	printstr("Start rest\n\r\0");
}

void timeplus() {
	seconds += 1;
	while (seconds>59) {
		seconds -= 60;
		minutes += 1;
		while (minutes>59) {
			minutes -= 60;
			hours += 1;
			while (hours>23) {
				hours -= 24;
			}
		}
	}

	if ((t_minutes>0) || (t_seconds>0)) {
		t_seconds -= 1;
		if (t_seconds>59) {
			t_seconds = 59;
			t_minutes -= 1;
		}
	} else {
		if (mode==2) {
			startrest();
		} else if (mode==3) {
			startwork();
		}
	}
}

void showtime() {
	switch (mode) {
		case 1: {
			defred = 16;
			defgreen = 4;
			defblue = 0;

			uint8_t sec10 = 0;
			uint8_t sec1 = seconds;
			while (sec1>9) {
				sec1 -= 10;
				sec10 += 1;
			}
			uint8_t min10 = 0;
			uint8_t min1 = minutes;
			while (min1>9) {
				min1 -= 10;
				min10 += 1;
			}
			setnum(0,min10);
			setnum(1,min1);
			setnum(2,sec10);
			setnum(3,sec1);
			setpt(0,0);
			setpt(1,0);
			break;
		}
		case 2: {
			defred = 32;
			defgreen = 0;
			defblue = 0;

			uint8_t sec10 = 0;
			uint8_t sec1 = t_seconds;
			while (sec1>9) {
				sec1 -= 10;
				sec10 += 1;
			}
			uint8_t min10 = 0;
			uint8_t min1 = t_minutes;
			while (min1>9) {
				min1 -= 10;
				min10 += 1;
			}
			setnum(0,min10);
			setnum(1,min1);
			setnum(2,sec10);
			setnum(3,sec1);
			setpt(0,1);
			setpt(1,0);
			break;
		}
		case 3: {
			defred = 0;
			defgreen = 0;
			defblue = 16;

			uint8_t sec10 = 0;
			uint8_t sec1 = t_seconds;
			while (sec1>9) {
				sec1 -= 10;
				sec10 += 1;
			}
			uint8_t min10 = 0;
			uint8_t min1 = t_minutes;
			while (min1>9) {
				min1 -= 10;
				min10 += 1;
			}
			setnum(0,min10);
			setnum(1,min1);
			setnum(2,sec10);
			setnum(3,sec1);
			setpt(0,0);
			setpt(1,1);
			break;
		}
		case 0:
		default:
		{
			defred = 16;
			defgreen = 4;
			defblue = 0;

			uint8_t min10 = 0;
			uint8_t min1 = minutes;
			while (min1>9) {
				min1 -= 10;
				min10 += 1;
			}
			uint8_t hr10 = 0;
			uint8_t hr1 = hours;
			while (hr1>9) {
				hr1 -= 10;
				hr10 += 1;
			}
			setnum(0,hr10);
			setnum(1,hr1);
			setnum(2,min10);
			setnum(3,min1);
			setpt(0,seconds&1);
			setpt(1,(~seconds)&1);

			printhex(hours);
			printhex(minutes);
			printhex(seconds);
			printstr("\n\r");
			break;
		}
	}





	needrefresh = 1;
}

void initTimer()
{
	TCCR1A = 0; //No compare
	TCCR1B = (1<<WGM12)|(0b101<<CS10); //CTC, clk/1024
	OCR1A = 15625+1; //16000000 / 1024 = 15625 (7812+7813)
	TIMSK1 = (1<<OCIE1A);
}

ISR(TIMER1_COMPA_vect)
{
//	cli();
	timeplus();
	needshow = 1;
//	sei();
}


void init() {
	initUSART();
	initTimer();
	SETD(LED);
	SETD(LEDCTRL);
	//SETP(LED);
	CLRD(PRGM);
	SETP(PRGM);
	j = 0;
	k = 0;
	needrefresh = 1;
	needshow = 1;
	clrscr();

//	setnum(0,2);
//	setnum(1,3);
//	setnum(2,2);
//	setnum(3,7);
	printstr("Pomodoro clock v0.1\n\r\0");

}

void chkpgm() {
	if (!PINV(PRGM)) {
		void *bl = (void *)0x3c00;
		goto *bl;
	}
}

void loop() {
	chkpgm();
	if (needrefresh) {
		needrefresh = 0;
		refresh();
	}
	if (needshow) {
		showtime();
		needshow = 0;
	}
	if (chrready()) {
		char ch = getchr();
		putchr(ch);
		switch (ch) {
			case 't':
			{
				hours = hextochar(getchr())*10;
				hours += hextochar(getchr());
				minutes = hextochar(getchr())*10;
				minutes += hextochar(getchr());
				seconds = hextochar(getchr())*10;
				seconds += hextochar(getchr());
				break;
			}
			case 'w':
			{
				t_workmin = hextochar(getchr())*10;
				t_workmin += hextochar(getchr());
				break;
			}
			case 'r':
			{
				t_restmin = hextochar(getchr())*10;
				t_restmin += hextochar(getchr());
				break;
			}
			case 'm':
			{
				mode = hextochar(getchr());
				break;
			}
			case 's':
			{
				startwork();
				break;
			}
			case 'a':
			{
				startrest();
				break;
			}
			default:
				break;
		}
	}


}

int main(void)
{
	init();
	while (1) {
		loop();
	}
	return 0;
}

