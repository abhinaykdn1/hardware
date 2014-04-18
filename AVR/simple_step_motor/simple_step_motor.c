/*
 * main.c
 *
 *  Created on: Jan 13, 2014
 *      Author: lim
 */
#include <avr/io.h>
#include <avr/interrupt.h>
//#include "ports.h"

//#define F_CPU 16000000

#define MOTOR_PORT PORTD
#define MOTOR_DDR DDRD
#define MOTOR_PHASES 4
#define MOTOR_PIN0 PD2
#define MOTOR_MASK ((~((~0)<<MOTOR_PHASES))<<MOTOR_PIN0)

uint16_t adc[2];
char phase = 0;
int counter = 0;
int increment = 0;

ISR(ADC_vect)
{
	PORTB ^= (1<<PORTB5); //blink led
	increment = 64;
	cli();

	int chnl = ADMUX & 0x07;
	int val = ADC;
	adc[chnl] = val;
	chnl = (chnl + 1) & 0x01;
	sei();
	ADMUX = (ADMUX & (~0x07)) | chnl;
//    ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADSC)|(1<<ADATE)|(3<<ADPS0);
}

//ISR(TIMER1_OVF_vect)
//{
//	PORTB ^= (1<<PORTB5);
//
//	char msk = ((1<<phase)<<MOTOR_PIN0);
//	char port = MOTOR_PORT;
//	port &= ~MOTOR_MASK;
//	port |= msk;
//	MOTOR_PORT = port;
//	phase = phase + 1;
//	if (phase >= MOTOR_PHASES)
//		phase = 0;
//}

ISR(TIMER1_COMPA_vect)
{
//	increment = adc[0] >> 4;
	counter += increment;
	phase = (counter >> 14) & 0b11;

	char msk = ((1<<phase)<<MOTOR_PIN0);
	char port = MOTOR_PORT;
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
//	TCCR1B = (0<<WGM12)|(0b001<<CS10); //Normal, clk/1
//	OCR1A = 5000; //16000000 / 8 / 5000 = 400
	OCR1A = 5; //16000000 / 8 / 5000 = 400000
	OCR1B = 0;
//	TIMSK1 = (1<<TOIE1);
	TIMSK1 = (1<<OCIE1A);
}

void init_adc()
{
//	PORTB ^= (1<<PORTB5); //blink led

    ADMUX = (1<<REFS0)|(1<<ADLAR)|(0<<MUX0); //Vref = Vcc, left adjust result
    ADCSRA = (1<<ADEN)|(1<<ADIE)|(3<<ADPS0)|(0<<ADATE);
//    ADCSRB = (5<<ADTS0); //Compare/match timer 1
//    sei();
//    ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADSC)|(1<<ADATE)|(3<<ADPS0);
//    ADCSRA = (7<<ADPS0)|(1<ADEN)|(1<<ADSC)|(1<<ADIE)|(1<<ADATE);
}

void adc_start()
{
//	PORTB ^= (1<<PORTB5); //blink led
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
