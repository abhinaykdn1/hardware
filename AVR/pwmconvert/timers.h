#include <avr/interrupt.h>

volatile unsigned int time0;
char time_changed = 0;
unsigned char last_in_portb = 0;
unsigned char last_in_portd = 0;
unsigned char last_out_portc = 0;
unsigned char last_out_portd = 0;


void inittimers(void)
{
	TIMSK0 |= bv(OCIE0A); //Interrupt on compare
	TCCR0A |= bv(WGM01); //CTC (clear on compare) mode

	TCCR0B = 0x01; //Prescaller clk/1
 
    OCR0A = TIMER0A_VALUE;
}

ISR (TIMER0_COMPA_vect)
{
	time0++;
	last_in_portb = PINB;
	last_in_portd = PIND;
	PORTC = last_out_portc;
	PORTD = last_out_portd;
//	PORTC ^= 0x02;
	time_changed = 1;

}
