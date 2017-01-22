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
#include "gpio.h"
#include "cfg500.h"
#include <avr/io.h>
#include <util/delay.h>
static unsigned char sck_dur=1;

#ifdef VAR_18CLK
static unsigned char d_sck_dur=9;
#else
static unsigned char d_sck_dur=6;
#endif

// the mapping between SCK freq and SCK_DURATION is for avrisp as follows:
// this table is for 14.7 MHz:
// sck_dur       sck-freq   d_sck_dur
// 0              921kHz     1
//                520kHz     2
//                370kHz     4
//                280kHz     6
// 1              230kHz     7
//                100kHz     22
// 2              57kHz      50
// 3              28kHz      80
// 4              27kHz      90
// 5              26kHz      100
// 7              20kHz      120
// 8              18kHz      140
// 10             15kHz      160
// 15             10kHz      240
// 19             9 kHz      254
// 25             6 kHz    
// 50             3 kHz   
// 100            1.5kHz  
unsigned char  spi_set_sck_duration(unsigned char dur)
{
        unsigned char j;
        if (dur >= 10){
        	// 10Khz
                sck_dur=15;
#ifdef VAR_18CLK
                d_sck_dur=253;
#else
                d_sck_dur=250;
#endif
        	goto RET_RESULT;
        }
        if (dur >= 7){
        	// 20Khz
                sck_dur=7;
#ifdef VAR_18CLK
                d_sck_dur=150;
#else
                d_sck_dur=120;
#endif
        	goto RET_RESULT;
        }
        if (dur >= 3){
        	// 28KHz
                sck_dur=3;
#ifdef VAR_18CLK
                d_sck_dur=100;
#else
                d_sck_dur=80;
#endif
        	goto RET_RESULT;
        }
        if (dur >= 2){
        	// 57KHz
                sck_dur=2;
#ifdef VAR_18CLK
                d_sck_dur=60;
#else
                d_sck_dur=50;
#endif
        	goto RET_RESULT;
        }
        if (dur >= 1){
        	// 230KHz
                sck_dur=1;
#ifdef VAR_18CLK
                d_sck_dur=9;
#else
                d_sck_dur=6;
#endif
        	goto RET_RESULT;
        }
        if (dur < 1 ){
                // 900KHz
                sck_dur=0;
#ifdef VAR_18CLK
                d_sck_dur=2;
#else
                d_sck_dur=1;
#endif
        	goto RET_RESULT;
        }
        // we should never come here
        sck_dur=1;
        d_sck_dur=9;
        //
RET_RESULT:
        return(sck_dur);
}

unsigned char spi_get_sck_duration(void)
{
        return(sck_dur);
}

void spi_sck_pulse(void)
{
	// SCK low
	bit_off(TG_SCK);
        _delay_loop_1(d_sck_dur);
        _delay_loop_1(d_sck_dur);
	// SCK high
	bit_on(TG_SCK);
        _delay_loop_1(d_sck_dur);
        _delay_loop_1(d_sck_dur);
	// SCK low
	bit_off(TG_SCK);
}

void spi_reset_pulse(void) 
{
        /* give a positive RESET pulse, we can't guarantee
         * that SCK was low during power up (see Atmel's
         * data sheets, search for "Serial Downloading in e.g atmega8 
         * data sheet):
         * "... the programmer can not guarantee that SCK is held low during
         * Power-up. In this case, RESET must be given a positive pulse of at least two
         * CPU clock cycles duration after SCK has been set to 0." 
         * */
	// reset = high = not active
	bit_on(TG_RESET);
        _delay_loop_1(d_sck_dur);
        _delay_loop_1(d_sck_dur);
	// reset = low, stay active
	bit_off(TG_RESET);
        delay_ms(20); // min stab delay
}

void spi_init(void) 
{
	// reset as output
	dir_out(TG_RESET);
	// +5V, reset off
	bit_on(TG_RESET);
	// MISO is input
	dir_in(TG_MISO);
	// now output pins low in case somebody used it as output in his/her circuit
	// MOSI is output
	dir_out(TG_MOSI);
	// MOSI low
	bit_off(TG_MOSI);
	// SCK is output
	dir_out(TG_SCK);
	// SCK low
	bit_off(TG_SCK);
        delay_ms(10); // discharge MOSI/SCK
	// reset = low, stay active
	bit_off(TG_RESET);
        delay_ms(20); // stab delay
        spi_reset_pulse();
}

// send 8 bit, no return value
void spi_mastertransmit_nr(unsigned char data)
{
        unsigned char i=128;
        // software spi
        while(i!=0){
                // MOSI
                if (data&i){
			bit_on(TG_MOSI);
                }else{
                        // transmit a zero
			bit_off(TG_MOSI);
                }
		_delay_loop_1(d_sck_dur);
		_delay_loop_1(1); // make pulse len equal to spi_mastertransmit
		// SCK high
		bit_on(TG_SCK);
		_delay_loop_1(d_sck_dur);
                // make low and high pulse equaly long:
                _delay_loop_1(3); 
		i=i>>1;
		// SCK low
		bit_off(TG_SCK);

        }
}

// send 8 bit, return received byte
unsigned char spi_mastertransmit(unsigned char data)
{
        unsigned char i=128;
        unsigned char rval=0;
        // software spi
        while(i!=0){
                // MOSI
                if (data&i){
			bit_on(TG_MOSI);
                }else{
                        // trans mit a zero
			bit_off(TG_MOSI);
                }
		_delay_loop_1(d_sck_dur);
                // read MISO
                if(bit_active(TG_MISO)){
                        rval|= i;
                }
		// SCK high
		bit_on(TG_SCK);
		_delay_loop_1(d_sck_dur);
		_delay_loop_1(3); // make low and high pulse equaly long
		i=i>>1;
		// SCK low
		bit_off(TG_SCK);
        }
        return(rval);
}

// send 16 bit, return last rec byte
void spi_mastertransmit_16_nr(unsigned int data)
{
	// SCK low
	bit_off(TG_SCK);
        spi_mastertransmit_nr((data>>8)&0xFF);
        spi_mastertransmit_nr(data&0xFF);
}

// send 32 bit, return last rec byte
unsigned char spi_mastertransmit_32(unsigned long data)
{
	// SCK low
	bit_off(TG_SCK);
        spi_mastertransmit_nr((data>>24)&0xFF);
        spi_mastertransmit_nr((data>>16)&0xFF);
        spi_mastertransmit_nr((data>>8)&0xFF);
        return(spi_mastertransmit(data&0xFF));
}

void spi_disable(void)
{
        // just to be sure:
	// SCK is output
	dir_out(TG_SCK);
	// SCK low
	bit_off(TG_SCK);
        //
	// +5V, reset off
	bit_on(TG_RESET);
        // all other pins as input:
	// MISO is input
	dir_in(TG_MISO);
	// pullup off
	bit_off(TG_MISO);
        //
	// SCK as input, high
	dir_in(TG_SCK);
	// pullup off
	bit_off(TG_SCK);
        //
	// MOSI as input, high
	dir_in(TG_MOSI);
	// pullup off
	bit_off(TG_MOSI);
        delay_ms(20); 
        // now we can also set reset high impedence:
	// reset as input, high
	dir_in(TG_RESET);
	// pullup off
	bit_off(TG_RESET);
}

