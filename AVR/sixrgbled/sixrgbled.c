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

int main(void)
{

	init();
	alloff();

	while (1)
	{
		//allon();

		//for (unsigned int i=0; i<4000; i++) {};

		//alloff();

		//for (unsigned int i=0; i<40000; i++) {};
		unsigned int rnd = rand();

		unsigned char con = rnd & 0x0007;
		unsigned char bon = (rnd & 0x01F8) >> 3;
		coloff(~con);
		colon(con);
		bitoff(~bon);
		biton(bon);

		for (unsigned int i=0; i<4000; i++) {};
		alloff();

		for (unsigned int i=0; i<40000; i++) {};
//		biton(0x02);

//		for (unsigned int i=0; i<40000; i++) {};
		
//		biton(0x04);

//		for (unsigned int i=0; i<40000; i++) {};

//		biton(0x08);

//		for (unsigned int i=0; i<40000; i++) {};

//		biton(0x10);

//		for (unsigned int i=0; i<40000; i++) {};

//		biton(0x20);

//		for (unsigned int i=0; i<40000; i++) {};


	}
}
