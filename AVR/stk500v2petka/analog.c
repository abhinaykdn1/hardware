/* vim: set sw=8 ts=8 si : */
/*
* ADC functions for atmega8.
* Author: Guido Socher, Copyright: GPL 
*/
#include <avr/io.h>
#include "gpio.h"
#include "spi.h" // todo delete this!
#include "timeout.h"
#include "analog.h"
#include "cfg500.h"
#include <util/delay.h>

static unsigned char config_do_reset_check=1;  // configurable via serial terminal
static unsigned char prg_state=0;  // 0=idle, 1=programming is ongoing

// return analog value of a given channel. Works without interrupt 
unsigned int convertanalog(unsigned char channel) 
{
	unsigned char adlow,adhigh;
        // enable analog to digital conversion in single run mode
        //  without noise canceler function. See datasheet of atmega88 page 250
        // We set ADPS2=1 ADPS0=1 ADPS1=1 to have a clock division factor of 128.
        // This is needed to stay in the recommended range of 50-200kHz 
	// Clock freq= 14MHz or 18 MHz
        // ADEN: Analog Digital Converter Enable
        // ADIE: ADC Interrupt Enable  (0=no interrupt)
        // ADIF: ADC Interrupt Flag
        // ADCSR: ADC Control and Status Register
        // ADPS2..ADPS0: ADC Prescaler Select Bits
	// REFS: Reference Selection Bits (page 203)
        ///

	// int-ref with external capacitor at AREF pin: 
	// atmega8: 2.56V int ref=REFS1=1,REFS0=1
	// atmega88: 1.1V int ref=REFS1=1,REFS0=1
	// write only the lower 3 bit for channel selection
	//
#ifdef USEAVCCREF
	// AVcc ref
	ADMUX=(1<<REFS0)|(channel & 0x0f);
#else
	// Use the intenal  ref:
	ADMUX=(1<<REFS1)|(1<<REFS0)|(channel & 0x0f);
#endif
	// 
#ifdef VAR_88CHIP
        ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
#else
        ADCSR=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
#endif
	// switch off digital input line:
	//DIDR0=(1<<channel)& 0x1f;

#ifdef VAR_88CHIP
	//  start conversion 
	ADCSRA|= (1<<ADSC);
	while(bit_is_set(ADCSRA,ADSC)); // wait for result 
#else
	//  start conversion 
	ADCSR|= (1<<ADSC);
	while(bit_is_set(ADCSR,ADSC)); // wait for result 
#endif
	adlow=ADCL; // read low first !! 
	adhigh=ADCH; 
	return((unsigned int)((adhigh<<8)|(adlow & 0xFF)));
}


unsigned char analog2v(unsigned int aval)
{
        unsigned int r;
#ifdef VAR_88CHIP
        //10* ref(=1.1) * divider(=5.4) / 10bit(=1023):
        r=(aval * 4 );
	r/=67;
#else
        //10* ref(=2.5) * divider(=3.1) / 10bit(=1023):
        r=(aval * 4 );
	r/=48;
#endif
        return((unsigned char)(r & 0xff));
}

unsigned char prg_state_get(void)
{
	return(prg_state);
}

void prg_state_set(unsigned char p)
{
	prg_state=p;
}

unsigned char config_get_reset_checkstate(void)
{
	return(config_do_reset_check);
}

void config_reset_checkstate(unsigned char p)
{
	config_do_reset_check=p;
}

unsigned char reset_works(void)
{
	unsigned char rvolt;
	unsigned char rvoltl;
	if (config_do_reset_check==0){
		return(1);
	}
	// first check the voltage with reset open
	// reset as input, high impedance
	dir_in(TG_RESET);
	// pullup off
	bit_off(TG_RESET);
	delay_ms(12);
	rvolt=analog2v(convertanalog(RESETADC)); // reset pin
	// now pull reset quickly to gnd and check if it is low
        // reset pin is output
	dir_out(TG_RESET);
        // reset = low
	bit_off(TG_RESET);
	delay_ms(12);
	rvoltl=analog2v(convertanalog(RESETADC)); // reset pin
	// reset as input, high impedance
	dir_in(TG_RESET);
	// pullup off
	bit_off(TG_RESET);
	if (rvolt/rvoltl > 3){
		return(1); // OK reset works
	}
	return(0); // fail
}

