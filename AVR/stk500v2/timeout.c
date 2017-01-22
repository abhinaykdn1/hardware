/* vim: set sw=8 ts=8 si et: */
/*
* Timer for timeout supervision of the stk 500 protocol
* Author: Guido Socher, Copyright: GPL V2
*/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include "timeout.h"
#include <util/delay.h>
#include "cfg500.h"
#include "prescaler.h"


void delay_ms(unsigned int ms)
/* delay for a minimum of <ms> */
{
        // we use a calibrated macro. This is more
        // accurate and not so much compiler dependent
        // as self made code.
        while(ms){
			wdt_reset();
			switch(getprescale()) {
			case 0: _delay_us(960); break;
			case 2: _delay_us(480); break;
			case 8: _delay_us(120); break;
			}
			ms--;
        }
}

void wd_init(void)
{
        // timeout the watchdog after 2 sec:
        wdt_enable(WDTO_2S);
        wdt_reset();
}


