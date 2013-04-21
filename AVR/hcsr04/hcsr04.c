#define F_CPU 16000000L
#define BAUDRATE 115200L
#define TIMER0A_VALUE 125-1

#define LED 5 //PC5
#define S_VCC 0 //PB0
#define S_GND 5 //PD5
#define S_TRIG 7 //PD7
#define S_ECHO 6 //PD6

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned int time;

void inittimers(void)
{
	time = 0;

	TIMSK0 |= (1<<OCIE0A);	//Interrupt on compare
	TCCR0A |= (1<<WGM01);	//CTC (clear on compare) mode
	TCCR0B = 0x04;			//Prescaller clk/256
 
    OCR0A = TIMER0A_VALUE;
}

ISR (TIMER0_COMPA_vect)
{
	cli();
	time++;
	sei();
}

void wait(unsigned int interval)
{
	unsigned int starttime=time;

	while (starttime+interval > time) {
	}
}


void init()
{
	DDRC |= (1<<LED);
	DDRB |= (1<<S_VCC);
	DDRD |= (1<<S_GND) || (1<<S_TRIG);
	DDRD &= ~(1<<(S_ECHO));

	PORTD &= ~((1<<S_GND) || (1<<S_TRIG));
	PORTB |= (1<<S_VCC);

	PORTC |= (1<<LED);

	inittimers();
	sei();
}


int main(void)
{
	init();

	while (1)
	{
//		for (int i=0; i<1; i++) {
			wait(500);
//		}
		//cli();
		PORTC ^= (1<<LED);
		//sei();
	}
}
