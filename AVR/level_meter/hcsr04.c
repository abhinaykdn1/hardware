/*
 * hcsr04.c
 *
 *  Created on: Sep 27, 2014
 *      Author: lim
 */

#include <avr/io.h>
#include "timer1.h"
#include "hcsr04.h"

void initHCSR04(void) {
	CLRP(HCSR04_ECHO);
	CLRD(HCSR04_ECHO);
	CLRP(HCSR04_TRIG);
	SETD(HCSR04_TRIG);
}

void trigger() {
	INVP(TXLED);
	SETP(HCSR04_TRIG);
	delay_us2(20); //10us
	CLRP(HCSR04_TRIG);
	INVP(TXLED);
}

uint16_t measure() {
	while (!PINV(HCSR04_ECHO)) {
		INVP(RXLED);
	}
	measureTime();
	while (PINV(HCSR04_ECHO)) {
		INVP(RXLED);
	}
	uint16_t result = measureTime();
	return result;
}

uint16_t getDistance() {
	trigger();
	return measure();
}
