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

	DDRA = 0xFF;
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD = 0xFF;

	// unsigned char r = 1;
	char shft = 0;
	while (1) {
		char rv = 1 << shft;
		shft = (shft + 1) & 0b111;

		PORTA = rv;
		PORTB = rv;
		PORTC = rv;
		PORTD = rv;

		wait(250);

		PORTA = 0;
		PORTB = 0;
		PORTC = 0;
		PORTD = 0;

		wait(750);
	}
}