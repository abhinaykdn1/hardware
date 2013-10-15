#include <avr/io.h>
#include <avr/interrupt.h>
#include "macro.h"

#define F_CPU 16000000L

#include "usart.h"

//#define TIMER0A_VALUE 125-1
#define TIMER0A_VALUE 32-1

#define LED 5 //PC5
#define S_TRIG 6 //PD6
#define S_ECHO 7 //PD7

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned int time;

void inittimers(void)
{
	time = 0;

	TIMSK0 |= (1<<OCIE0A);	//Interrupt on compare
	TCCR0A |= (1<<WGM01);	//CTC (clear on compare) mode
//	TCCR0B = 0x04;			//Prescaller clk/256
	TCCR0B = 0x03;			//Prescaller clk/64
 
    OCR0A = TIMER0A_VALUE;
}

ISR (TIMER0_COMPA_vect)
{
	cli();
	time++;
	sei();
}

void resettime()
{
	cli();
	time = 0;
	sei();
}

void wait(unsigned int interval)
{
//	unsigned int starttime=time;

	cli();
	time = 0;
	sei();
	while (interval > time) {
	}
}


void init()
{
	DDRC |= (1<<LED);
//	DDRB |= (1<<S_VCC);
//	DDRD |= (1<<S_GND);
	DDRD |= (1<<S_TRIG);
	DDRD &= ~(1<<(S_ECHO));

	PORTD &= ~(1<<S_TRIG);
//	PORTD |= (1<<S_GND);
//	PORTB &= ~(1<<S_VCC);

	PORTC |= (1<<LED);

	inittimers();
	initUSART();
	sei();
}


int main(void)
{
	init();

	printstr("HC-SR04 Sonar 0.1\n\r\0");
//	while (1)
//	{
//		PORTC ^= (1<<LED);
//		wait(100);
//	}

	while (1)
	{
		PORTC ^= (1<<LED);
		wait(1);
		PORTC ^= (1<<LED);

//		printstr(".\0");
//		PORTC &= ~(1<<LED);
//		wait(1);
//		PORTC |= (1<<LED);

//		for (int i = 0; i < 200; i++) {

			PORTD |= (1<<S_TRIG);
//			wait(1);
			for (int j = 0; j < 20; j++) {}
			PORTD &= ~(1<<S_TRIG);

			int wd = 16384;
			short pin = (PIND & (1<<S_ECHO));
			while ((pin == 0) && (wd > 0)) {
				pin = (PIND & (1<<S_ECHO));
				wd--;
			}
			resettime();

			wd = 16384;
			while ((pin != 0) && (wd > 0)) {
				pin = (PIND & (1<<S_ECHO));
				wd--;
			}

			if (wd>0) {
				cli();
				unsigned int measuredtime = time;
				sei();

				printhex((measuredtime>>8)&0xFF);
				printhex(measuredtime&0xFF);
				printstr("\n\r\0");
			}

			wait(10000);

//		}

//		PORTC &= ~(1<<LED);
//		wait(1);
//		PORTC |= (1<<LED);




//		wait(500);
//		PORTD &= ~(1<<S_GND);
//		PORTB |= (1<<S_VCC);
//		wait(500);
//		PORTD |= (1<<S_GND);
//		PORTB &= ~(1<<S_VCC);

//		PORTC ^= (1<<LED);
//		wait(5);
//		PORTC ^= (1<<LED);
//		wait(50);
//		PORTC ^= (1<<LED);
//		wait(5);
//		PORTC ^= (1<<LED);
	}
}
