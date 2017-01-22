

#include "exclock.h"
#include "cfg500.h"
#include <avr/io.h>
#include <avr/eeprom.h>
#include "analog.h"
#include "timeout.h"
#include "events.h"



unsigned char OscPscale;
unsigned char OscCmatch;

unsigned char EEMEM eeprom_OscPscale = 0;
unsigned char EEMEM eeprom_OscCmatch = 0;


char m = 1;
int c;

#define	F_CLK 14745600    // clock frequency, MHz
#define	T_SYS 1000     // system tick, us
#define	F_CLK_STK 7372800 // clock frequency of original STK, MHz. Do not change!






void SetOscOut(void)
{
	m = 1;
	switch(OscPscale) {
	case 3: OscPscale = 2; m = 4; break; // CK / 32 = CK / 8 / 4
	case 4: OscPscale = 3; break;        // CK / 64
	case 5: OscPscale = 3; m = 2; break; // CK / 128 = CK / 64 / 2
	case 6: OscPscale = 4; break;        // CK / 256
	case 7: OscPscale = 5; break;        // CK / 1024
	}
	
	TCCR1B = (1 << WGM12) | (OscPscale & 7); // CTC, prescaler
	
	if(OscPscale) { //If clock generator is active
		if( GetTargetStat() ) EXCLOCK_BUFF_ON; //Ext clock bufer is on
		TCCR1A = (1 << COM1A0); // toggle OC1A on compare
	} else { //If clock generator is stopped
		EXCLOCK_BUFF_OFF; //Ext clock bufer is Hi-Z
		TCCR1A = 0; //ext. clock off
	}
	
	//int c = ((int)OscCmatch + 1) * (long)(F_CLK) / (long)(F_CLK_STK);
	c = ((int)OscCmatch + 1) * 2;
	OCR1A = c * m - 1;
  
	if ( (eeprom_read_byte(&eeprom_OscPscale)) != OscPscale)
	eeprom_write_byte(&eeprom_OscPscale, OscPscale); //write to eeprom
  
	if ( (eeprom_read_byte(&eeprom_OscCmatch)) != OscCmatch)
	eeprom_write_byte(&eeprom_OscCmatch, OscCmatch); //write to eeprom

}


void OscOutReInit0(void) {
	c = ((int)OscCmatch + 1) * 2;
	OCR1A = c * m - 1;
}

void OscOutReInit2(void) {
	c = ((int)OscCmatch + 1);
	OCR1A = c * m - 1;
}

/*
void OscOutReInit8(void) {
	c = ((int)OscCmatch + 1) / 4;
	OCR1A = c * m - 1;
}
*/





void set_osc_pscale(unsigned char t) {
	OscPscale = t;	
}

void set_osc_cmatch(unsigned char t) {
	OscCmatch = t;
	SetOscOut();
}

unsigned char get_osc_pscale(void) {
	return OscPscale;
}

unsigned char get_osc_cmatch(void) {
	return OscCmatch;
}


void ExtClock_Init(void) {

	EXCLOCKBUFF_DDR |= (1<<EXCLOCKBUFF_PIN);
	EXCLOCKBUFF_PORT &= ~(1<<EXCLOCKBUFF_PIN);

  
	//OC1A as output
	DDRB |= (1<<PB1);
	PORTB &= ~(1<<PB1);
  
	OscPscale = eeprom_read_byte(&eeprom_OscPscale);
	OscCmatch = eeprom_read_byte(&eeprom_OscCmatch);
  
	// setup timer 2 as system timer:
	TCCR2A = (1 << WGM21) | (1 << CS22) | (1 << CS20); // CTC, CK / 128
	//OCR2A = (int)(F_CLK / T_SYS / 128) - 1;
	OCR2A = (int)(F_CLK / T_SYS / 128) - 1;
  
	SetOscOut();
  
}
