#include <avr/io.h>
#include <avr/interrupt.h>
//#include "macro.h"

#define F_CPU 16000000

void task0(void)
{
}

void task1(void)
{
}

void init(void)
{
	void (*link)(void) = &task0;
	(*link)();	
}

int main(void)
{
	init();
	return 0;
}
