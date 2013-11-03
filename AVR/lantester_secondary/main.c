#include <avr/io.h>
#include <avr/interrupt.h>
#include "ports.h"

volatile long time = 0;
volatile int wtime = 0;

ISR(TIMER2_COMP_vect)
{
	cli();
	time++;
	wtime++;
    sei();
}

void initTimer2()
{
	TCCR2 = (1<<WGM21)|(0<<WGM20)|(0b011<<CS20); //CTC mode, prescaller CLK/64
 	OCR2 = 249; // 16 000 00 Hz / 64 / 250 = 1000 Hz
 	TIMSK = (1<<OCIE2); //Interrupt on second timer compare
}

void wait(int tms)
{
	wtime = 0;
	while (wtime < tms) {
	}
}

void init()
{
	initTimer2();
	sei();
}

int main()
{
	init();

	DDRD = 0xFF;
	PORTD = 0;
	wait(100);
	char rp = 0;
	char rv = 0;

	while (1) {
		for (char shft = 0; shft < 8; shft++)
		{
			rp = 0b11111111 << shft;

			//even bits
			rv = rp & 0b01010101;
			PORTD = rv;
			wait(5);
			PORTD = 0;
			wait(5);

			//odd bits
			rv = rp & 0b10101010;
			PORTD = rv;
			wait(5);
			PORTD = 0;
			wait(5);
		}
	}
}
	


/*
 0 1 2 3 4 5 6 7

 1 0 1 0 1 0 1 0
 0 1 0 1 0 1 0 1
 0 0 1 0 1 0 1 0
 0 1 0 1 0 1 0 1
 0 0 1 0 1 0 1 0
 0 0 0 1 0 1 0 1
 0 0 0 0 1 0 1 0
 0 0 0 1 0 1 0 1
 0 0 0 0 1 0 1 0
 0 0 0 0 0 1 0 1
 0 0 0 0 0 0 1 0
 0 0 0 0 0 1 0 1
 0 0 0 0 0 0 1 0
 0 0 0 0 0 0 0 1
 0 0 0 0 0 0 0 0
 0 0 0 0 0 0 0 1





*/