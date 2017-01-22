/* vim: set sw=8 ts=8 si : */
/*
* ADC functions for atmega8.
* Author: Guido Socher, Copyright: GPL 
*/
#include <avr/io.h>
#include "analog.h"
#include "cfg500.h"



//ADC init
void adc_init(void) {

	DDRC &= ~(1<<ADCPIN); // пин ацп на вход
	PORTC &= ~(1<<ADCPIN); 

	ADMUX=(1<<REFS1)|(1<<REFS0)|(ADCPIN & 0x0f); // внутренний 1.1v источник опорного напряжения
	ADCSRA=(1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);	
		
}


// return analog value of a given channel. Works without interrupt 
unsigned int convertanalog(void) {
	unsigned char res = (ADCW*4)/67; //66 - calibrate!
	return (res);
}


