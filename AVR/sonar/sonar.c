#define F_CPU 14745000L
#define F_TARGET 40000
#define BAUDRATE 19200L
#include <avr/io.h>
#include <avr/interrupt.h>
#include "macro.h"
#include "usart.h"
#include "timers.h"
//#define F_INT 10

#define US_ON 5 //Negative ultrasonic pin
#define US_OP 6 //Positive ultrasonic pin
//#define OUT3 5
//#define OUT4 4
#define US_OPORT PORTD
#define US_ODDR DDRD

#define US_I 0
#define US_IPORT PORTA
#define US_IDDR DDRA
#define US_IPIN PINA

#define IMPULSES 10
#define TIME_HL 0
#define TIME_EM (TIME_HL + IMPULSES*2) //emit time in half/periods
#define TIME_RL (TIME_EM + 20) //relax time
//#define TIME_LS (TIME_RL + 4700) //listen time
#define TIME_LS (TIME_RL + 600) //listen time

volatile unsigned int time0; //
char umode = 1; //0 - off, 1 - emit, 2 - wait and discharge transmitter, 3 - recieve
char uinlast = 0;
unsigned int impcount = 0;
 
unsigned int timest = 0; //time when signal appear
unsigned int timeen = 0; //time when signal disappear
void initIO(void)
{
    US_ODDR |= bv(US_ON) | bv(US_OP);
	US_IDDR &= ~(bv(US_I));
}
 


void setumode(int m)
{
	if (umode != m) {
		cli();
		switch (m) {
			case 0: {
				US_ODDR &= ~(bv(US_OP) | bv(US_ON));
				time0 = 0;
				break;
			}
			case 1: { //emit
				US_OPORT |= bv(US_OP);
				US_OPORT &= ~(bv(US_ON));
			    US_ODDR |= bv(US_ON) | bv(US_OP);
				break;
			}
			case 2: {
				US_OPORT &= ~(bv(US_ON) | bv(US_OP)); //Discharge piezo to ground
			    US_ODDR |= bv(US_ON) | bv(US_OP);
				break;
			}
			case 3: { //listen
				US_ODDR &= ~(bv(US_OP) | bv(US_ON));
				uinlast = (US_IPIN & (bv(US_I))) != 0;
				impcount = 0;
				timest = 0;
				timeen = 0;
				break;
			}
		}
		umode = m;
		sei();
	}
}


void init(void)
{
        initUSART();
        initIO();
        initOC2();
		US_OPORT |= bv(US_OP);
		US_OPORT &= ~(bv(US_ON));
		time0 = 0;
}

int main(void)
{

	init();

	while (1)
	{
	//	cli();
	//	ibi(OUT_PORT, OUT1);
	//	sei();
		if (time0 > TIME_LS) {
			setumode(0);
		} else if (time0 > TIME_RL) {
			setumode(3);
		} else if (time0 > TIME_EM) {
			setumode(2);
		} else if (time0 > TIME_HL) {
			setumode(1);
		}
	}
}


/*
ISR (TIMER0_OVF_vect)
{
	tt0++;
	if (tt0%2==1) {
		sbi(OUT_PORT, OUT2);
	} else {
		cbi(OUT_PORT, OUT2);	
	}

}
*/

ISR (TIMER2_COMP_vect)
{
	switch (umode) {
		case 1: {
			US_OPORT ^= bv(US_OP) | bv(US_ON);
			break;
		}
		case 3: {
			char uincur = (US_IPIN & (bv(US_I))) != 0; //get true or false on input pin
			if (uincur != uinlast) { //if was change during last half period
				uinlast = uincur;
				impcount++; //then increase impulse count
			} else {
				if (impcount<IMPULSES) { //otherwise if impulse count below minimum
					impcount = 0; //then wrong work, it was may be ghost :)
					timest = time0;
				} else { //otherwise we got result in pulses
					umode = 0; //finish listening here
					timeen = time0;
				}
			}
			break;
		}
	}

	time0++;
}

