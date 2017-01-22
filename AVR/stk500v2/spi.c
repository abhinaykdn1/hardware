/* vim: set sw=8 ts=8 si et: */
/*
* Serial Peripheral Interface for avrusb500 v2
* We do all spi operations in software. This 
* gives more flexibility. The atmega8 is at 14MHz fast
* enough to do this. The C-code here is highly optimized
* for speed. 
* Check the result with: avr-objdump -d main.out
* Author: Guido Socher, Copyright: GPL V2
*/
#include "timeout.h" // must be first
#include "spi.h"
#include "cfg500.h"
#include "led.h"
#include <avr/io.h>
#include <util/delay.h>
#include "prescaler.h"
#include "analog.h"
#include <avr/eeprom.h>









static unsigned char prg_state=0;  // 0=idle, 1=programming is ongoing

unsigned char sck_dur; // 0-4 (0=1.843MHz; 1=460.8kHz; 2=115.2kHz; 3=57.6kHz; 4=14.75kHz)
unsigned char reset_polarity = 1; // 0 - AT89**, 1 - other (if RSTINVERT definined always 1)

//Init values as eeprom
unsigned char EEMEM eeprom_sck_dur = 2; //default sck_dur value

static unsigned int d_sck_dur = 445; // Program SPI speed (445 = 4KHz)






unsigned char prg_state_get(void) {
	return(prg_state);
}

void prg_state_set(unsigned char p) {
	prg_state=p;
}












#ifndef RSTINVERT
void set_reset_polarity(char t) {
	reset_polarity = t;
}

char get_reset_polarity(void) {
	return reset_polarity;
}
#endif


void set_spi_speed(char t) {

	switch(t) {
	case 0: //1843200 (8)	
		SPCR |= (1<<SPE); //enable hardware spi
		SPCR &= ~(1<<SPR1);
		SPCR |= (1<<SPR0);	
		SPSR |= (1<<SPI2X);
	break;
	case 1: //460800 (32)
		SPCR |= (1<<SPE); //enable hardware spi
		SPCR |= (1<<SPR1);
		SPCR &= ~(1<<SPR0);
		SPSR |= (1<<SPI2X);
	break;
	case 2: //115200,  (128)
	case 3: //57600
		SPCR |= (1<<SPE); //enable hardware spi
		SPCR |= (1<<SPR1);
		SPCR |= (1<<SPR0);
		SPSR &= ~(1<<SPI2X);
	break;
	case 4: //4000 (software)
		SPCR &= ~(1<<SPE); //disasle hardware spi
	break;
	}

}


void spi_init(void) {
	
	
	//Bufers off here (0)
	
	sck_dur = eeprom_read_byte(&eeprom_sck_dur); //reed sck speed from eeprom
	
	BUFERS_EN_DDR |= (1<<BUFERS_EN_PIN);
	BUFERS_EN_PORT &= ~(1<<BUFERS_EN_PIN);

	//all pins as input:
	MISO_IN_P;
	MOSI_IN_P;
	SCK_IN_P;
	RESET_IN_P;

		
	//Для правильной работы SPI пин SS должен быть настроен на выход
	DDRB |= 1<<PB2;
	PORTB  &= ~ (1<<PB2);
	
	
	
	SPCR = (1<<MSTR);
	SPSR = 0x00;
	set_spi_speed(sck_dur);
	
	
	
	
	
}





unsigned char spi_get_sck_duration(void) {
	if (sck_dur >= 4)
	 return 0x4B;
	else
	 return(sck_dur);
}


void spi_set_sck_duration(unsigned char dur) { //Установка скорости ISP

	if(dur > 3)
	 sck_dur = 4;
	else
	 sck_dur = dur;
	
	if ( (eeprom_read_byte(&eeprom_sck_dur)) != sck_dur)
	eeprom_write_byte(&eeprom_sck_dur, sck_dur); //write to eeprom

	set_spi_speed(sck_dur);
}


void spi_start(void) {
	prg_state_set(1);
		
		MISO_IN;
        MOSI_OUT;
        SCK_OUT;
		
		BUFFS_ON;
		
		RESET_OUT;
		#ifndef RSTINVERT
			if(reset_polarity) {
			 RESET_0;
			} else {
			 RESET_1;
			}
		#else
			RESET_1; //inverted
		#endif
		

        

	
		if(sck_dur == 3) prescale2();
}








// send 8 bit, no return value
void software_spi_mastertransmit_nr(unsigned char data)
{
	unsigned char i=128;
	// software spi
	while(i!=0) {
		
		if (data&i) // MOSI
		 PORTB |= 1<<PB3;
		else
		 PORTB &= ~(1<<PB3);
			
		_delay_loop_2(d_sck_dur);
		_delay_loop_1(1); // make pulse len equal to spi_mastertransmit
		
		PORTB|= (1<<PB5); // SCK high
		_delay_loop_2(d_sck_dur);
		i=i>>1;
		PORTB &= ~(1<<PB5); // SCK low
			
	}
}

// send 8 bit, return received byte
unsigned char software_spi_mastertransmit(unsigned char data)
{
	unsigned char i=128;
	unsigned char rval=0;
	// software spi
	while(i!=0) {
	
		if (data&i) // MOSI
		 PORTB |= 1<<PB3;
		else
		 PORTB &= ~(1<<PB3);
	
	_delay_loop_2(d_sck_dur);
	//read MISO
	if(PINB & (1<<PB4)) rval |= i;
	
	PORTB|= (1<<PB5); // SCK high
	_delay_loop_2(d_sck_dur);
	i=i>>1;
	PORTB &= ~(1<<PB5); // SCK low
	
	}
	return(rval);
}



// send 8 bit, no return value
void hardware_spi_mastertransmit_nr(unsigned char data) {
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
}


// send 8 bit, return received byte
unsigned char hardware_spi_mastertransmit(unsigned char data) {
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}






// send 8 bit, no return value
void spi_mastertransmit_nr(unsigned char data) {
	if(sck_dur == 4)
	 software_spi_mastertransmit_nr(data);
	else
	 hardware_spi_mastertransmit_nr(data);
}


// send 8 bit, return received byte
unsigned char spi_mastertransmit(unsigned char data) {
	if(sck_dur == 4)
	 return software_spi_mastertransmit(data);
	else
	 return hardware_spi_mastertransmit(data);
}





// send 16 bit, no return value
void spi_mastertransmit_16_nr(unsigned int data) {

	spi_mastertransmit_nr((data>>8)&0xFF);
	spi_mastertransmit_nr(data&0xFF);
}


// send 32 bit, return last rec byte
unsigned char spi_mastertransmit_32(unsigned long data) {

	spi_mastertransmit_nr((data>>24)&0xFF);
	spi_mastertransmit_nr((data>>16)&0xFF);
	spi_mastertransmit_nr((data>>8)&0xFF);
	return(spi_mastertransmit(data&0xFF));
}


void spi_stop(void) {
	prg_state_set(0);
	
	if(sck_dur == 3) prescale0();
	
	BUFFS_OFF;
	
	MISO_IN_P;
	MOSI_IN_P;
	SCK_IN_P;
	RESET_IN_P;

	
}

