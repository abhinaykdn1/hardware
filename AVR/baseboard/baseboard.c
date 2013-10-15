#include <avr/io.h>
#include <avr/interrupt.h>
#include "macro.h"

#define F_CPU 8000000

#include "uart.h"

#define ADC_BUF_SIZE 8

unsigned int adc_buf[ADC_BUF_SIZE];

void initADC()
{
	ADMUX = 1<<REFS0|1<<ADLAR|0<<MUX0;
	sei();
	ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADSC)|(0<<ADATE)|(3<<ADPS0);
}

void initPins()
{
	DDRD = 1<<PORTD4 | 1<<PORTD5;
	PIND = 0x00;
}

ISR(ADC_vect)
{
	int chnl = ADMUX & 0x07;
	int val = ADC;
	adc_buf[chnl] = val;
	chnl = (chnl + 1) & 0x07;
	ADMUX = (ADMUX & (~0x07)) | chnl;
	ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADSC)|(0<<ADATE)|(3<<ADPS0);
}

void init()
{
	initUSART();
	initADC();
	initPins();
}

int main(void)
{
	init();
	printstr("Baseboard0.1\n\r\0");

	int i = 0;
	while (1) {
		i++;
		if (chrready()) {
			char ch = getchr();
			switch (ch) {
				case '?':
				{
					printstr("BaseBoard0.1\n\r\0");
					break;
				}
				case '0':
				case '1':
//				case '2':
//				case '3':
//				case '4':
//				case '5':
				case '6':
				case '7':
				{
					unsigned int a = adc_buf[ch-'0'];
					char ah = (a >> 8) & 0xFF;
					char al = a & 0xFF;
					printhex(ah);
					printhex(al);
					printstr("\n\r\0");
					break;
				}
				case 'a':
				case 'A':
				{
					sbi(PORTD,PORTD4);
					printstr("D4 on\n\r\0");
					break;
				}

				case 'b':
				case 'B':
				{
					cbi(PORTD,PORTD4);
					printstr("D4 off\n\r\0");
					break;
				}
				case 'c':
				case 'C':
				{
					sbi(PORTD,PORTD5);
					printstr("D5 on\n\r\0");
					break;
				}

				case 'd':
				case 'D':
				{
					cbi(PORTD,PORTD5);
					printstr("D5 off\n\r\0");
					break;
				}
			}
		}
	}
}
