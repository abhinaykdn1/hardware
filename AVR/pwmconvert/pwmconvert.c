#include "config.h"
#include "macro.h"
#include <avr/io.h>

#include "uart.h"
#include "timers.h"


void init(void)
{
	inituart();
	inittimers();
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
		PORTC ^= 0x01;
	}
}
