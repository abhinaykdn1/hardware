#include "config.h"
#include "macro.h"
#include <avr/io.h>

#include "uart.h"
#include "timers.h"

unsigned char outpwm[OUT_PWM_COUNT];
unsigned char inpwm[IN_PWM_COUNT];


void init(void)
{
	for (short i=0; i < IN_PWM_COUNT; i++) {
		inpwm[i] = 0;
	}
	for (short i=0; i < OUT_PWM_COUNT; i++) {
		outpwm[i] = 0;
	}

	DDRB &= ~IN_PORTB_MASK;
	DDRD &= ~IN_PORTD_MASK;

	DDRC |= OUT_PORTC_MASK;
	DDRD |= OUT_PORTD_MASK;

	inituart();
	inittimers();
	sei();
}

int main(void)
{
	init();

	DDRC = 0xFF;

	putchr(' ');
	putchr('V');
	putchr('0');
	putchr('.');
	putchr('0');
	putchr('0');
	while (1)
	{
		if (time_changed) {
			time_changed = 0;
			last_out_portc ^= bv(OUT1);
		}

		cli();
		PORTC ^= 0x01;
		sei();
	}
}
