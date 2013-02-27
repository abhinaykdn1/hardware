#include "config.h"
#include "macro.h"
#include <avr/io.h>

#include "uart.h"
#include "timers.h"

unsigned char outpwm[OUT_PWM_COUNT];
unsigned char outpwmtmp[OUT_PWM_COUNT];
unsigned char inpwm[IN_PWM_COUNT];


void init(void)
{
	for (short i=0; i < IN_PWM_COUNT; i++) {
		inpwm[i] = 0;
	}
	for (short i=0; i < OUT_PWM_COUNT; i++) {
		outpwm[i] = 0;
		outpwmtmp[i] = 0;
	}

	outpwm[0] = 1;
	outpwm[1] = 20;
	outpwm[2] = 100;


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

	int time1 = 0;
	while (1)
	{
		if (time_changed) {
			time_changed = 0;
			
			time1 = time0 && 0x01FF; //mod 512
			if (time1==0) {
				for (short i=0; i < OUT_PWM_COUNT; i++) {
					outpwmtmp[i] = outpwm[i];
				}
			} else {
				for (short i=0; i < OUT_PWM_COUNT; i++) {
					if (outpwmtmp[i]>0) {
						outpwmtmp[i]--;
						switch (i) {
							case 0:
								{
									last_out_portc |= bv(OUT0);
									break;
								}
							case 1:
								{
									last_out_portc |= bv(OUT1);
									break;
								}
							case 2:
								{
									last_out_portc |= bv(OUT2);
									break;
								}
							case 3:
								{
									last_out_portc |= bv(OUT3);
									break;
								}
							case 4:
								{
									last_out_portc |= bv(OUT4);
									break;
								}
							case 5:
								{
									last_out_portc |= bv(OUT5);
									break;
								}
							case 6:
								{
									last_out_portd |= bv(OUT6);
									break;
								}
							case 7:
								{
									last_out_portd |= bv(OUT7);
									break;
								}
						}
					} else {
						switch (i) {
							case 0:
								{
									last_out_portc &= ~bv(OUT0);
									break;
								}
							case 1:
								{
									last_out_portc &= ~bv(OUT1);
									break;
								}
							case 2:
								{
									last_out_portc &= ~bv(OUT2);
									break;
								}
							case 3:
								{
									last_out_portc &= ~bv(OUT3);
									break;
								}
							case 4:
								{
									last_out_portc &= ~bv(OUT4);
									break;
								}
							case 5:
								{
									last_out_portc &= ~bv(OUT5);
									break;
								}
							case 6:
								{
									last_out_portd &= ~bv(OUT6);
									break;
								}
							case 7:
								{
									last_out_portd &= ~bv(OUT7);
									break;
								}
						}
					}

					
				}
			}
			
			last_out_portc ^= bv(OUT1);
		}

		cli();
		PORTD ^= bv(LED0);
		sei();
	}
}
