//#include <avr/io.h>

//#include "config.h"

//#if defined(__AVR_ATmega48__)

/*
 * Timer 0 hit TOP (0xff), i.e. it turns from up-counting
 * into down-counting direction.
 */

 /*
.global TIMER0_COMPA_vect
TIMER0_COMPA_vect:
//	PUSH      R1            // Push register on stack
//	PUSH      R0            // Push register on stack
//	IN        R0,0x3F       // In from I/O location
//	PUSH      R0            // Push register on stack
//	CLR       R1            // Clear Register
	PUSH      R24           // Push register on stack
	PUSH      R25           // Push register on stack

	LDS       R24,time0     // Load direct from data space
	LDS       R25,time0+1   // Load direct from data space
	ADIW      R24,0x01      // Add immediate to word
	STS       time0+1,R25   // Store direct to data space
	STS       time0,R24     // Store direct to data space

	IN        R24,0x03      // In from I/O location
	STS       last_in_portb,R24    // Store direct to data space
	IN        R24,0x09      // In from I/O location
	STS       last_in_portd,R24    // Store direct to data space
	LDS       R24,last_out_portc    // Load direct from data space
	OUT       0x08,R24      // Out to I/O location
	LDS       R24,last_out_portd    // Load direct from data space
	OUT       0x0B,R24      // Out to I/O location
	LDI       R24,0x01      // Load immediate
	STS       time_changed,R24    // Store direct to data space
	POP       R25           // Pop register from stack
	POP       R24           // Pop register from stack
//	POP       R0            // Pop register from stack
//	OUT       0x3F,R0       // Out to I/O location
//	POP       R0            // Pop register from stack
//	POP       R1            // Pop register from stack
	RETI                    // Interrupt return
*/
