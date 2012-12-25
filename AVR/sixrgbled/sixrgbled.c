#define F_CPU 8000000L
//#define F_TARGET 44100
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "macro.h"
//PC2 PC3 PC4
//PD5 PB7 PB6


//PC0 PC1 PC2 // anodes,
//PD7 PD6 PD5 // on=0

//PB2 PB1 PB0 //RGB cathodes, on=1


void init(void)
{
	DDRB |= bv(0) | bv(1) | bv(2);
	DDRC |= bv(0) | bv(1) | bv(2);
	DDRD |= bv(5) | bv(6) | bv(7);

	DDRB |= 0xff;
	DDRC |= 0xff;
	DDRD |= 0xff;

}

void allon(void)
{
	PORTB |= bv(0) | bv(1) | bv(2);
	PORTC &= ~(bv(0) | bv(1) | bv(2));
	PORTD &= ~(bv(5) | bv(6) | bv(7));
}


void alloff(void)
{
	PORTB &= ~(bv(0) | bv(1) | bv(2));
	PORTC |= bv(0) | bv(1) | bv(2);
	PORTD |= bv(5) | bv(6) | bv(7);
}

void biton(unsigned char bits)
{
	unsigned char pc = bits & 0x07;
	unsigned char pd = (bits & 0x38) << 2;
	PORTC &= ~pc;
	PORTD &= ~pd;
}

void bitoff(unsigned char bits)
{
	unsigned char pc = bits & 0x07;
	unsigned char pd = (bits & 0x38) << 2;
	PORTC |= pc;
	PORTD |= pd;
}

void bitset(unsigned char bits)
{
	bitoff(~bits);
	biton(bits);

}


void colon(unsigned char bits) // 1-green, 2-blue, 4-red
{
	unsigned char pb = bits & 0x07;
	PORTB |= pb;
}

void coloff(unsigned char bits)
{
	unsigned char pb = bits & 0x07;
	PORTB &= ~pb;
}

void colset(unsigned char bits)
{
	coloff(~bits);
	colon(bits);
}

int main(void)
{

//	char r[6] = {0x00, 0x10, 0x20, 0x40, 0x80, 0xf0};
//	char g[6] = {0x00, 0x10, 0x20, 0x40, 0x80, 0xf0};
//	char b[6] = {0x00, 0x10, 0x20, 0x40, 0x80, 0xf0};

	char r[6] = {0xff, 0xff, 0xA0, 0x00, 0x00, 0x00};
	char g[6] = {0x00, 0x10, 0xA0, 0xff, 0xA0, 0x00};
	char b[6] = {0x00, 0x00, 0x00, 0x00, 0xA0, 0xff};

	init();
	alloff();

	char ir = 0;
	char ig = 0;
	char ib = 0;

	while (1)
	{

		colset(1);
		for (unsigned char i = 0; i<255; i++) {
			unsigned char bits = 0;
			for (unsigned char j = 0; j<6; j++) {
				bits = bits << 1;
				bits |= (g[j]>i);
				g[j]+=ig;
			}
			bitset(bits);
		}
		alloff();

		colset(2);
		for (unsigned char i = 0; i<255; i++) {
			unsigned char bits = 0;
			for (unsigned char j = 0; j<6; j++) {
				bits = bits << 1;
				bits |= (b[j]>i);
				b[j]+=ib;
			}
			bitset(bits);
		}
		alloff();

		colset(4);
		for (unsigned char i = 0; i<255; i++) {
			unsigned char bits = 0;
			for (unsigned char j = 0; j<6; j++) {
				bits = bits << 1;
				bits |= (r[j]>i);
				r[j]+=ir;
			}
			bitset(bits);
		}
		alloff();
	}
}
