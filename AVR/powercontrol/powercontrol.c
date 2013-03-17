#include <avr/io.h>
#include <avr/interrupt.h>
#include "macro.h"

#define F_CPU 14745600

#include "usart.h"

#define ADC_BUF_SIZE 8

int adc_buf[ADC_BUF_SIZE];

void initADC()
{
	ADMUX = 1<<REFS0|1<<ADLAR|0<<MUX0;
	sei();
	ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADSC)|(0<<ADATE)|(3<<ADPS0);
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
}

int main(void)
{
	init();
	printstr("Test\n\r\0");
	printstr(">\0");
	int i = 0;
	while (1) {
		i++;
		if (chrready()) {
			char ch = getchr();
			switch (ch) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				{
					int a = adc_buf[ch-'0'];
					char ah = (a >> 8) & 0xFF;
					char al = a & 0xFF;
					printhex(ah);
					printhex(al);
					break;
				}
			}
		}
	}
}
