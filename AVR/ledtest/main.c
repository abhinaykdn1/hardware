/*
 * main.c
 *
 *  Created on: Oct 3, 2014
 *      Author: lim
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "ports.h"
#include "timer1.h"

#define LED0 PORTA_D2
#define LED1 PORTA_D3
#define LED2 PORTA_D4
#define LED3 PORTA_D5
#define LED4 PORTA_D6
#define LED5 PORTA_D7
#define LED6 PORTA_D8
#define LED7 PORTA_D9


void init(void) {
	SETD(TXLED);
	SETD(RXLED);
	SETD(LED0);
	SETD(LED1);
	SETD(LED2);
	SETD(LED3);
	SETD(LED4);
	SETD(LED5);
	SETD(LED6);
	SETD(LED7);

#ifdef __AVR_ATmega32U4__
	USBCON = (0<<USBE)|(1<<FRZCLK)|(0<<OTGPADE)|(0<<VBUSTE);
#endif
	initTimer1();
	sei();
}

int main(void) {
	init();

	uint16_t delay = 200;
	while (1) {
		INVP(LED0);
		delay_ms(delay);
		INVP(LED1);
		delay_ms(delay);
		INVP(LED2);
		delay_ms(delay);
		INVP(LED3);
		delay_ms(delay);
		INVP(LED4);
		delay_ms(delay);
		INVP(LED5);
		delay_ms(delay);
		INVP(LED6);
		delay_ms(delay);
		INVP(LED7);
		delay_ms(delay);
	}

	return 0;
}

#ifdef __AVR_ATmega32U4__
ISR(USB_COM_vect) {
}

ISR(USB_GEN_vect) {
}
#endif
