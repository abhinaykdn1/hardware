#define F_CPU 8000000L
#define F_TARGET 44100
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "macro.h"
//#include "timers.h"
#include "sin.h"

#define LED0 4
#define LED0P PORTD
#define LED1 3
#define LED1P PORTD
#define LED2 2
#define LED2P PORTD
#define LED3 1
#define LED3P PORTD
#define LED4 0
#define LED4P PORTD
#define LED5 6
#define LED5P PORTC
#define LED6 5
#define LED6P PORTC
#define LED7 4
#define LED7P PORTC
#define LED8 3
#define LED8P PORTC
#define LED9 2
#define LED9P PORTC
#define LEDA 1
#define LEDAP PORTC
#define LEDB 0
#define LEDBP PORTC
#define PWR 2
#define PWRP PORTB

#define LED0ON  LED0P |=  bv(LED0)
#define LED0OFF LED0P &= ~bv(LED0)
#define LED1ON  LED1P |=  bv(LED1)
#define LED1OFF LED1P &= ~bv(LED1)
#define LED2ON  LED2P |=  bv(LED2)
#define LED2OFF LED2P &= ~bv(LED2)
#define LED3ON  LED3P |=  bv(LED3)
#define LED3OFF LED3P &= ~bv(LED3)
#define LED4ON  LED4P |=  bv(LED4)
#define LED4OFF LED4P &= ~bv(LED4)
#define LED5ON  LED5P |=  bv(LED5)
#define LED5OFF LED5P &= ~bv(LED5)
#define LED6ON  LED6P |=  bv(LED6)
#define LED6OFF LED6P &= ~bv(LED6)
#define LED7ON  LED7P |=  bv(LED7)
#define LED7OFF LED7P &= ~bv(LED7)
#define LED8ON  LED8P |=  bv(LED8)
#define LED8OFF LED8P &= ~bv(LED8)
#define LED9ON  LED9P |=  bv(LED9)
#define LED9OFF LED9P &= ~bv(LED9)
#define LEDAON  LEDAP |=  bv(LEDA)
#define LEDAOFF LEDAP &= ~bv(LEDA)
#define LEDBON  LEDBP |=  bv(LEDB)
#define LEDBOFF LEDBP &= ~bv(LEDB)

void init(void)
{
        //initIO();
        //initOC2();
		//US_OPORT |= bv(US_OP);
		//US_OPORT &= ~(bv(US_ON));
		//time0 = 0;
	DDRB |= bv(PWR);
	PWRP |= bv(PWR);
	DDRD |= bv(LED0) | bv(LED1) | bv(LED2) | bv(LED3) | bv(LED4);
	DDRC |= bv(LED5) | bv(LED6) | bv(LED7) | bv(LED8) | bv(LED9) | bv(LEDA) | bv(LEDB);
	//PORTD |= bv(LED0) | bv(LED1) | bv(LED2) | bv(LED3) | bv(LED4);
	//PORTC |= bv(LED5) | bv(LED6) | bv(LED7) | bv(LED8) | bv(LED9) | bv(LEDA) | bv(LEDB);
}

void ledon(int l)
{
	switch (l%12) {
		case 0: {
			LED0ON;
			break;
		}
		case 1: {
			LED1ON;
			break;
		}
		case 2: {
			LED2ON;
			break;
		}
		case 3: {
			LED3ON;
			break;
		}
		case 4: {
			LED4ON;
			break;
		}
		case 5: {
			LED5ON;
			break;
		}
		case 6: {
			LED6ON;
			break;
		}
		case 7: {
			LED7ON;
			break;
		}
		case 8: {
			LED8ON;
			break;
		}
		case 9: {
			LED9ON;
			break;
		}
		case 10: {
			LEDAON;
			break;
		}
		case 11: {
			LEDBON;
			break;
		}
	}
}

void ledoff(int l)
{
	switch (l%12) {
		case 0: {
			LED0OFF;
			break;
		}
		case 1: {
			LED1OFF;
			break;
		}
		case 2: {
			LED2OFF;
			break;
		}
		case 3: {
			LED3OFF;
			break;
		}
		case 4: {
			LED4OFF;
			break;
		}
		case 5: {
			LED5OFF;
			break;
		}
		case 6: {
			LED6OFF;
			break;
		}
		case 7: {
			LED7OFF;
			break;
		}
		case 8: {
			LED8OFF;
			break;
		}
		case 9: {
			LED9OFF;
			break;
		}
		case 10: {
			LEDAOFF;
			break;
		}
		case 11: {
			LEDBOFF;
			break;
		}
	}
}

void pwroff(void)
{
	PORTD = 0;
	PORTC = 0;
	DDRD = 0;
	DDRC = 0;

	PWRP &= ~bv(PWR); //auro power off

}

int main(void)
{

	init();

	int k = 0;

	for (int j = 0; j<255; j++)
	{
		//int i;
//		k = rand();
		k = rand();
		ledon(k);
		for (unsigned int i = 0; i<10000; i++) {};
		ledoff(k);
//		PORTD |= bv(LED0) | bv(LED1) | bv(LED2) | bv(LED3) | bv(LED4);
//		PORTC |= bv(LED5) | bv(LED6) | bv(LED7) | bv(LED8) | bv(LED9) | bv(LEDA) | bv(LEDB);
		
		for (unsigned int i = 0; i<40000; i++) {};

//		PORTD &= ~(bv(LED0) | bv(LED1) | bv(LED2) | bv(LED3) | bv(LED4));
//		PORTC &= ~(bv(LED5) | bv(LED6) | bv(LED7) | bv(LED8) | bv(LED9) | bv(LEDA) | bv(LEDB));

//		for (i = 0; i<40000; i++) {};

	}

	pwroff();
}
