/*
 * main.c
 *
 *  Created on: Jan 13, 2014
 *      Author: lim
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ports.h"

//#define F_CPU 16000000

#define MOTOR_PORT PORTD
#define MOTOR_DDR DDRD
#define MOTOR_PHASES 4
#define MOTOR_PIN0 PD2
#define MOTOR_MASK ((~((~0)<<MOTOR_PHASES))<<MOTOR_PIN0)

uint16_t adc[2];
uint8_t phase = 0;
uint8_t lastmsk = 0;
uint16_t hldtime = 0;
uint16_t counter = 0;
int16_t increment = 0;
uint16_t pause = 0;

void adc_start();

ISR(ADC_vect)
{
	int chnl = ADMUX & 0x07;
	int val = ADC;
	adc[chnl] = val;
	chnl = (chnl + 1) & 0x01;

	ADMUX = (ADMUX & (~0x07)) | chnl;

	adc_start();
}

ISR(TIMER1_COMPA_vect)
{
	increment = (adc[0] >> 6) - 512;
	counter += increment;
	phase = (counter >> 14) & 0b11;

	uint8_t msk = (((0b11001<<phase)>>3)<<MOTOR_PIN0);
	if (msk == lastmsk) {
		if (hldtime > 4000) {
			msk = 0;
		} else {
			hldtime++;
		}
	} else {
		hldtime = 0;
		lastmsk = msk;
		PORTB ^= (1<<PORTB5); //blink led
	}

	uint8_t port = MOTOR_PORT;
	port &= ~MOTOR_MASK;
	port |= msk;
	MOTOR_PORT = port;
//	phase = phase + 1;
//	if (phase >= MOTOR_PHASES)
//		phase = 0;
}

void init_motor()
{
	MOTOR_PORT = (MOTOR_PORT & (~MOTOR_MASK));
	MOTOR_DDR = (MOTOR_DDR | MOTOR_MASK);
}

void init_timer()
{
	TCCR1A = 0; //No compare
	TCCR1B = (1<<WGM12)|(0b100<<CS10); //CTC, clk/256
	OCR1A = 2 ; //16000000 / 256 / 2 = 12500
	TIMSK1 = (1<<OCIE1A);
}

void init_adc()
{
    ADMUX = (1<<REFS0)|(1<<ADLAR)|(0<<MUX0); //Vref = Vcc, left adjust result
    ADCSRA = (1<<ADEN)|(1<<ADIE)|(3<<ADPS0)|(0<<ADATE);
}

void adc_start()
{
	ADCSRA |= (1<<ADSC);
}

void init()
{
    counter = 0;
    increment = -1;
	init_motor();
	init_adc();
	init_timer();
	sei();
}

int main(void)
{
	init();
	adc_start();
	DDRB |= (1<<PORTB5); //LED
//	PORTB ^= (1<<PORTB5); //blink led

	while (1) {
	}

	return 0;
}
