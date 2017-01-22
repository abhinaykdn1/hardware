
#include <avr/io.h>
#include "exclock.h"


char prescale = 0;
unsigned int count = 65530; //timeout if connection error

char getprescale(void) {
	return prescale;
}

void prescale0(void) {//Выключаем деление частоты
	//F_CPU 14745600
	prescale = 0;
	
	count = 65530;
	
	OscOutReInit0();

	CLKPR=(1<<CLKPCE); // change enable
	CLKPR=(0b0000 & 0x0f); // pre-scaler = 0
	
	UCSR0A =  (0<<U2X0);
	
	UBRR0H = (unsigned char) (7 >> 8);
	UBRR0L = (unsigned char) (7);
}

void prescale2(void) {//Деление частоты на два
	//7372800
	prescale = 2;
	
	count = 32765;
	
	OscOutReInit2();

	CLKPR=(1<<CLKPCE); // change enable
	CLKPR=(0b0001 & 0x0f); // pre-scaler = 2
	
	UBRR0H = (unsigned char) (3 >> 8);
	UBRR0L = (unsigned char) (3);
}

/*
void prescale8(void) 
{
	//1843200
	prescale = 8;
	
	count = 8192;
	
	OscOutReInit8();
	
	CLKPR=(1<<CLKPCE); // change enable
	CLKPR=(0b0011 & 0x0f); // pre-scaler = 8
	
	UCSR0A =  (1<<U2X0); //uart speed x2
	
	UBRR0H = (unsigned char) (1 >> 8);
	UBRR0L = (unsigned char) (1);
}
*/


