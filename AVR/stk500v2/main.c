/* vim: set sw=8 ts=8 si et: */
/*********************************************
* An implemenation of the STK500 specification (appl. note AVR068) for atmega8
* * Author: Guido Socher, Copyright: GPL
*
* Terminal capabilities for updating SW/HW version:
* Idea from Florin-Viorel Petrov, www.fonitzu.com/electronics, Date: 28Feb2006
* Florin-Viorel's version was a bit more fancy than the version implemented
* here. This implemenation is just small and simple.
*
* Copyright: GPL
**********************************************/
#include "timeout.h" // must be first
#include <inttypes.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "uart.h"
#include "analog.h"
#include "led.h"
#include "spi.h"
#include "avr_compat.h"
#include "command.h"
#include "cfg500.h"
#include "prescaler.h"
#include "events.h"
#include "exclock.h"

#define CONFIG_PARAM_BUILD_NUMBER_LOW   0
#define CONFIG_PARAM_BUILD_NUMBER_HIGH  1
// be careful with changing HW versions. avrstudio does not like all numbers:
#define CONFIG_PARAM_HW_VER             0x02  //Hardware Revision
// update here our own default sw version:
#define CONFIG_PARAM_SW_MAJOR           0x02     //Firmware Version (high byte) 0x02
#define CONFIG_PARAM_SW_MINOR           0x0a     //Firmware Version (low byte) 0x0a
#define CONFIG_PARAM_VADJUST 00 //25
//#define CONFIG_PARAM_OSC_PSCALE 0
//#define CONFIG_PARAM_OSC_CMATCH 1


#define MSG_IDLE 0
#define MSG_WAIT_SEQNUM 1
#define MSG_WAIT_SIZE1 2
#define MSG_WAIT_SIZE2 3
#define MSG_WAIT_TOKEN 4
#define MSG_WAIT_MSG 5
#define MSG_WAIT_CKSUM 6


// handling off addressed larger than 64k words
static unsigned char larger_than_64k=0;
static unsigned char new_address=0;

static unsigned char extended_address=0;
static unsigned long address=0;
static uint16_t saddress=0;

#define BUF_SIZE 275 //275 is a maximum for the STK500
static unsigned char msg_buf[BUF_SIZE];

static unsigned char param_controller_init=0;


/* transmit an answer back to the programmer software, message is
 * in msg_buf, seqnum is the seqnum of the last message from the programmer software,
 * len=1..275 according to avr068 */
void transmit_answer(unsigned char seqnum,unsigned int len)
{
        unsigned char cksum;
        unsigned char ch;
        int i;
        if (len>BUF_SIZE || len <1){
                // software error
                len = 2;
                // msg_buf[0]: not changed
                msg_buf[1] = STATUS_CMD_FAILED;
        }
        uart_sendchar(MESSAGE_START); // 0x1B
        cksum = MESSAGE_START^0;
        uart_sendchar(seqnum);
        cksum^=seqnum;
        ch=(len>>8)&0xFF;
        uart_sendchar(ch);
        cksum^=ch;
        ch=len&0xFF;
        uart_sendchar(ch);
        cksum^=ch;
        uart_sendchar(TOKEN); // 0x0E
        cksum^=TOKEN;
        wdt_reset();
        for(i=0;i<len;i++) {
			uart_sendchar(msg_buf[i]);
			cksum^=msg_buf[i];
        }
        uart_sendchar(cksum);
		
}

void programcmd(unsigned char seqnum)
{
		LED3_ON;
        unsigned char tmp,tmp2,addressing_is_word,cj,cstatus;
		unsigned int ci;
        unsigned int answerlen;
        unsigned int poll_address=0;
        unsigned int i,nbytes;
        // distingush addressing CMD_READ_EEPROM_ISP (8bit) and CMD_READ_FLASH_ISP (16bit)
        addressing_is_word=1; // 16 bit is default
		
		

        switch(msg_buf[0]) {
		
                case CMD_SIGN_ON:
                // prepare answer:
                msg_buf[0] = CMD_SIGN_ON; // 0x01
                msg_buf[1] = STATUS_CMD_OK; // 0x00
                msg_buf[2] = 8; //len
                strcpy((char *)&(msg_buf[3]),"STK500_2"); // note: this copies also the null termination (AVRISP_2 or STK500_2)
                answerlen=11;
                break;

                case CMD_SET_PARAMETER:
                switch(msg_buf[1]) {
					case PARAM_SCK_DURATION:
							spi_set_sck_duration(msg_buf[2]);
							break;
					case PARAM_CONTROLLER_INIT:
							param_controller_init=msg_buf[2];
							break;
					#ifndef RSTINVERT
					case PARAM_RESET_POLARITY:
							set_reset_polarity(msg_buf[2]);	
							break;
					#endif
					case PARAM_OSC_PSCALE:
							set_osc_pscale(msg_buf[2]);
							break;
					case PARAM_OSC_CMATCH:
							set_osc_cmatch(msg_buf[2]);
							break;
				}
				answerlen = 2;
				msg_buf[1] = STATUS_CMD_OK;
                //msg_buf[0] = CMD_SET_PARAMETER;
                break;		

                case CMD_GET_PARAMETER:
                tmp2=0;
                switch(msg_buf[1]) {
					case PARAM_BUILD_NUMBER_LOW:
							tmp = CONFIG_PARAM_BUILD_NUMBER_LOW;
							break;
					case PARAM_BUILD_NUMBER_HIGH:
							tmp = CONFIG_PARAM_BUILD_NUMBER_HIGH;
							break;
					case PARAM_HW_VER:
							tmp = CONFIG_PARAM_HW_VER;
							break;
					case PARAM_SW_MAJOR:
							tmp = CONFIG_PARAM_SW_MAJOR;
							break;
					case PARAM_SW_MINOR:
							tmp = CONFIG_PARAM_SW_MINOR;
							break;
					case PARAM_VTARGET:
							tmp=convertanalog(); //get voltage
							break;
					case PARAM_VADJUST:
							tmp = CONFIG_PARAM_VADJUST;
							break;
					case PARAM_SCK_DURATION:
							tmp = spi_get_sck_duration();
							break;
					case PARAM_CONTROLLER_INIT:
							tmp = param_controller_init;
							break;
					case PARAM_OSC_PSCALE:
							tmp = get_osc_pscale();
							break;
					case PARAM_OSC_CMATCH:
							tmp = get_osc_cmatch();
							break;
					case PARAM_TOPCARD_DETECT: // stk500 only
							tmp = 0xff; // no card
							break;
					case PARAM_DATA: // stk500 only
							tmp = 0; 
							break;
					default:
							tmp2=1; //return error
							break;
                }
                if (tmp2 == 0) { //if command successed
						answerlen = 3;
                        //msg_buf[0] = CMD_GET_PARAMETER;
                        msg_buf[1] = STATUS_CMD_OK;
                        msg_buf[2] = tmp;
                } else { //if command fail
                       // command not understood
                        answerlen = 2;
                        //msg_buf[0] = CMD_GET_PARAMETER;
                        msg_buf[1] = STATUS_CMD_UNKNOWN; 
                }
                break;
				
				case CMD_OSCCAL:
					answerlen = 2;
					msg_buf[1] = STATUS_CMD_FAILED;
				break;

                case CMD_LOAD_ADDRESS:
                address =  ((unsigned long)msg_buf[1])<<24;
                address |= ((unsigned long)msg_buf[2])<<16;
                address |= ((unsigned long)msg_buf[3])<<8;
                address |= ((unsigned long)msg_buf[4]);
                // atmega2561/atmega2560
                //If bit 31 is set, this indicates that the following read/write operation
                //will be performed on a memory that is larger than 64KBytes. This is an
                //indication to STK500 that a load extended address must be executed. See
                //datasheet for devices with memories larger than 64KBytes.
                //
                if (msg_buf[1] >= 0x80) {
                        larger_than_64k = 1;
                }else{
                        larger_than_64k = 0;
                }
                extended_address = msg_buf[2];
                new_address = 1;
                answerlen = 2;
                //msg_buf[0] = CMD_LOAD_ADDRESS;
                msg_buf[1] = STATUS_CMD_OK;
                break;

                case CMD_FIRMWARE_UPGRADE:
					answerlen = 2;
					msg_buf[1] = STATUS_CMD_FAILED;
                break;

                case CMD_ENTER_PROGMODE_ISP: // 0x10
                // The syntax of this command is as follows:
                // 0: Command ID 1 byte, CMD_ENTER_ PROGMODE_ISP
                // 1: timeout 1 byte, Command time-out (in ms)
                // 2: stabDelay 1 byte, Delay (in ms) used for pin stabilization
                // 3: cmdexeDelay 1 byte, Delay (in ms) in connection with the EnterProgMode command execution
                // 4: synchLoops 1 byte, Number of synchronization loops
                // 5: byteDelay 1 byte, Delay (in ms) between each byte in the EnterProgMode command.
                // 6: pollValue 1 byte, Poll value: 0x53 for AVR, 0x69 for AT89xx
                // 7: pollIndex 1 byte, Start address, received byte: 0 = no polling, 3 = AVR, 4 = AT89xx
                // cmd1 1 byte
                // cmd2 1 byte
                // cmd3 1 byte
                // cmd4 1 byte
				
				answerlen=2;
                msg_buf[1] = STATUS_CMD_FAILED;
				
				if( !GetTargetStat() ) break; //error due if target not connected
						
				spi_start();
				
                //delay_ms(msg_buf[2]); // stabDelay
				delay_ms(20); // stabDelay

                i=0;
				
				if (msg_buf[4]<=0) msg_buf[4]=1; //минимальное количество попыток подключения (1)
                if (msg_buf[4]>15) msg_buf[4]=15; //максимальное количество попыток подключения (15)
				
                while(i<msg_buf[4]) {//synchLoops
                        wdt_reset();
                        i++;
						//delay_ms(msg_buf[3]); //cmdexeDelay
                        spi_mastertransmit_nr(msg_buf[8]);//cmd1
						//delay_ms(msg_buf[5]); //byteDelay
                        spi_mastertransmit_nr(msg_buf[9]); //cmd2
						//delay_ms(msg_buf[5]); //byteDelay
                        tmp = spi_mastertransmit(msg_buf[10]);//cmd3
						//delay_ms(msg_buf[5]); //byteDelay
                        tmp2 = spi_mastertransmit(msg_buf[11]);//cmd4	

						//7=pollIndex, 6=pollValue
                        if(msg_buf[7]==3 && tmp==msg_buf[6]) { //tmp==msg_buf[6]
                                msg_buf[1] = STATUS_CMD_OK;
                        } else if(msg_buf[7]!=3 && tmp2==msg_buf[6]) {
                                msg_buf[1] = STATUS_CMD_OK;
                        } else if(msg_buf[7]==0) { //pollIndex
                                msg_buf[1] = STATUS_CMD_OK;
                        }
						
                        if(msg_buf[1] == STATUS_CMD_OK) {
							break; // end loop
                        } else {
							
							//пытаемся подключится к программируемому устройству
							//--------------------------------
							#ifndef RSTINVERT
								if(get_reset_polarity()) {
									RESET_1;
								} else {
									RESET_0;
								}
								
								delay_ms(20);
								
								if(get_reset_polarity()) {
									RESET_0;
								} else {
									RESET_1;
								}
								
								delay_ms(20);
							#else
								RESET_0;
								delay_ms(20);
								RESET_1;
								delay_ms(20);
							#endif
							//--------------------------------
							
							
                        }

                }
                break;
                
				
                case CMD_LEAVE_PROGMODE_ISP:
                spi_stop();
                answerlen = 2;
                msg_buf[1] = STATUS_CMD_OK;
                break;

                case CMD_CHIP_ERASE_ISP:     
                spi_mastertransmit_nr(msg_buf[3]);
                spi_mastertransmit_nr(msg_buf[4]);
                spi_mastertransmit_nr(msg_buf[5]);
                spi_mastertransmit_nr(msg_buf[6]);
                if(msg_buf[2]==0) {
                        // pollMethod use delay
                        delay_ms(msg_buf[1]); // eraseDelay
                } else {
                        // pollMethod RDY/BSY cmd
                        ci=300; // timeout
                        while((spi_mastertransmit_32(0xF0000000)&1)&&ci){
                                ci--;
                        }
                }
                answerlen = 2;
                //msg_buf[0] = CMD_CHIP_ERASE_ISP;
                msg_buf[1] = STATUS_CMD_OK;
                break;

                case CMD_PROGRAM_EEPROM_ISP:
                addressing_is_word=0;  // address each byte
                case CMD_PROGRAM_FLASH_ISP:
                // msg_buf[0] CMD_PROGRAM_FLASH_ISP = 0x13
                // msg_buf[1] NumBytes H
                // msg_buf[2] NumBytes L
                // msg_buf[3] mode
                // msg_buf[4] delay
                // msg_buf[5] cmd1 (Load Page, Write Program Memory)
                // msg_buf[6] cmd2 (Write Program Memory Page)
                // msg_buf[7] cmd3 (Read Program Memory)
                // msg_buf[8] poll1 (value to poll)
                // msg_buf[9] poll2
                // msg_buf[n+10] Data
                poll_address=0;
		ci=300;
                // set a minimum timed delay
                if (msg_buf[4] < 4){
                        msg_buf[4]=4;
                }
                // set a max delay
                if (msg_buf[4] > 32){
                        msg_buf[4]=32;
                }
		saddress=(address&0xffff); // previous address, start address 
		nbytes = (unsigned int)( (msg_buf[1]<<8) |msg_buf[2]);
                if (nbytes> 280){
                        // corrupted message
                        answerlen = 2;
                        msg_buf[1] = STATUS_CMD_FAILED;
                        break;
                }
                // store the original mode:
                tmp2=msg_buf[3];
                // result code
				
				cstatus=STATUS_CMD_OK;
                
				
                // msg_buf[3] test Word/Page Mode bit:  
                if ((msg_buf[3]&1)==0){
                        // word mode
                        for(i=0;i<nbytes;i++)
                        {        
                                // The Low/High byte selection bit is
                                // bit number 3. Set high byte for uneven bytes
                                if(addressing_is_word && i&1) {
                                        // high byte
                                        spi_mastertransmit_nr(msg_buf[5]|(1<<3));
                                } else {
                                        // low byte 
                                        spi_mastertransmit_nr(msg_buf[5]);
                                }
                                spi_mastertransmit_16_nr(address&0xffff);
                                spi_mastertransmit_nr(msg_buf[i+10]);
                                // if the data byte is not same as poll value
                                // in that case we can do polling:
                                if(msg_buf[8]!=msg_buf[i+10]) {
                                        poll_address = address&0xFFFF;
                                        // restore the possibly modifed mode:
                                        msg_buf[3]=tmp2;
                                } else {
                                        //switch the mode to timed delay (waiting), for this word
                                        msg_buf[3]= 0x02;
                                }
                                //
                                wdt_reset();
                                if (!addressing_is_word){
                                        // eeprom writing, eeprom needs more time
                                        delay_ms(2);
                                }
                                //check the different polling mode methods
                                if(msg_buf[3]& 0x04) {
                                        //data value polling
                                        tmp=msg_buf[8];
                                        ci=500; // timeout
                                        while(tmp==msg_buf[8] && ci ){
                                                // The Low/High byte selection bit is
                                                // bit number 3. Set high byte for uneven bytes
                                                // Read data:
                                                if(addressing_is_word && i&1) {
                                                        spi_mastertransmit_nr(msg_buf[7]|(1<<3));
                                                } else {
                                                        spi_mastertransmit_nr(msg_buf[7]);
                                                }
                                                spi_mastertransmit_16_nr(poll_address);
                                                tmp=spi_mastertransmit(0x00);
                                                ci--;
                                        }
                                } else if(msg_buf[3]&0x08){
                                        //RDY/BSY polling
                                        ci=500; // timeout
                                        while((spi_mastertransmit_32(0xF0000000)&1)&&ci){
                                                ci--;
                                        }
                                }else{
                                        //timed delay (waiting)
                                        delay_ms(msg_buf[4]);
                                }
                                if (addressing_is_word){
                                        //increment word address only when we have an uneven byte
                                        if(i&1) address++;
                                }else{
                                        //increment address
                                        address++;
                                }
				if (ci==0) {
					cstatus=STATUS_CMD_TOUT;
				}
                        }                        
                }else{
                        //page mode, all modern chips, atmega etc...
                        i=0;
                        while(i < nbytes){
                                wdt_reset();
                                // In commands PROGRAM_FLASH and READ_FLASH "Load Extended Address" 
                                // command is executed before every operation if we are programming 
                                // processor with Flash memory bigger than 64k words and 64k words boundary 
                                // is just crossed or new address was just loaded.
                                if (larger_than_64k && ((address&0xFFFF)==0 || new_address)){
                                        // load extended addr byte 0x4d
                                        spi_mastertransmit(0x4d);
                                        spi_mastertransmit(0x00);
                                        spi_mastertransmit(extended_address);
                                        spi_mastertransmit(0x00);
                                        new_address = 0;
                                }
                                // The Low/High byte selection bit is
                                // bit number 3. Set high byte for uneven bytes
                                if(addressing_is_word && i&1) {
                                        spi_mastertransmit_nr(msg_buf[5]|(1<<3));
                                } else {
                                        spi_mastertransmit_nr(msg_buf[5]);
                                }
                                spi_mastertransmit_16_nr(address&0xffff);
                                spi_mastertransmit_nr(msg_buf[i+10]);
                                
                                // the data byte is not same as poll value
                                // in that case we can do polling:
                                if(msg_buf[8]!=msg_buf[i+10]) {
                                        poll_address = address&0xFFFF;
                                } else {
                                        //switch the mode to timed delay (waiting)
                                        //we must preserve bit 0x80
                                        msg_buf[3]= (msg_buf[3]&0x80)|0x10;
                                
                                }
                                if (addressing_is_word){
                                        //increment word address only when we have an uneven byte
                                        if(i&1) {
                                                address++;
                                                if((address&0xFFFF)==0xFFFF){ 
                                                        extended_address++;
                                                }
                                        }
                                }else{
                                        address++;
                                }
                                i++;
                        }
                        //page mode check result:
                        //
                        // stk sets the Write page bit (7) if the page is complete
                        // and we should write it.
                        if(msg_buf[3]&0x80) {
                                spi_mastertransmit_nr(msg_buf[6]);
                                spi_mastertransmit_16_nr(saddress);
                                spi_mastertransmit_nr(0);
                                //
                                if (!addressing_is_word){
                                        // eeprom writing, eeprom needs more time
                                        delay_ms(1);
                                }
                                //check the different polling mode methods
                                ci=500; // timeout
                                if(msg_buf[3]&0x20 && poll_address) {
                                        //Data value polling
                                        tmp=msg_buf[8];
                                        while(tmp==msg_buf[8] && ci){
                                                // The Low/High byte selection bit is
                                                // bit number 3. Set high byte for uneven bytes
                                                // Read data:
                                                if(poll_address&1) {
                                                        spi_mastertransmit_nr(msg_buf[7]|(1<<3));
                                                } else {
                                                        spi_mastertransmit_nr(msg_buf[7]);
                                                }
                                                spi_mastertransmit_16_nr(poll_address);
                                                tmp=spi_mastertransmit(0x00);
                                                ci--;
                                        }
                                        if (ci==0){
                                                cstatus=STATUS_CMD_TOUT;
                                        }
                                } else if(msg_buf[3]& 0x40){
                                        //RDY/BSY polling
                                        while((spi_mastertransmit_32(0xF0000000)&1)&&ci){
                                                ci--;
                                        }
                                        if (ci==0){
                                                cstatus=STATUS_RDY_BSY_TOUT;
                                        }
                                }else{
                                        // simple waiting
                                        delay_ms(msg_buf[4]);
                                }
                        }
                }
                answerlen = 2;
                //msg_buf[0] = CMD_PROGRAM_FLASH_ISP; or CMD_PROGRAM_EEPROM_ISP
                msg_buf[1] = cstatus;
                break;

                case CMD_READ_EEPROM_ISP:
                addressing_is_word=0;  // address each byte
                case CMD_READ_FLASH_ISP:
		// msg_buf[1] and msg_buf[2] NumBytes
		// msg_buf[3] cmd
		nbytes = ((unsigned int)msg_buf[1])<<8;
		nbytes |= msg_buf[2];
		tmp = msg_buf[3];
                // limit answer len, prevent overflow:
                if (nbytes> 280){
                        nbytes=280;
                }
                //
                i=0;     
                while(i<nbytes)
		{
                        wdt_reset();
                        // In commands PROGRAM_FLASH and READ_FLASH "Load Extended Address" 
                        // command is executed before every operation if we are programming 
                        // processor with Flash memory bigger than 64k words and 64k words boundary 
                        // is just crossed or new address was just loaded.
                        if (larger_than_64k && ((address&0xFFFF)==0 || new_address)){
                                // load extended addr byte 0x4d
                                spi_mastertransmit(0x4d);
                                spi_mastertransmit(0x00);
                                spi_mastertransmit(extended_address);
                                spi_mastertransmit(0x00);
                                new_address = 0;
                        }
			//Select Low or High-Byte
			if(addressing_is_word && i&1) {
				spi_mastertransmit_nr(tmp|(1<<3));
			} else {
				spi_mastertransmit_nr(tmp);
			}
			
			spi_mastertransmit_16_nr(address&0xffff);
			msg_buf[i+2] = spi_mastertransmit(0);
			
                        if (addressing_is_word){
                                //increment word address only when we have an uneven byte 
                                if(i&1) {
                                        address++;
                                        if((address&0xFFFF)==0xFFFF){
                                                extended_address++;
                                        }
                                }
                        }else{
                                address++;
                        }
                        i++;
		}
		answerlen = nbytes+3;
		//msg_buf[0] = CMD_READ_FLASH_ISP; or CMD_READ_EEPROM_ISP
		msg_buf[1] = STATUS_CMD_OK;
		msg_buf[nbytes+2] = STATUS_CMD_OK;
                break;

                case CMD_PROGRAM_LOCK_ISP:
                case CMD_PROGRAM_FUSE_ISP:  
                spi_mastertransmit_nr(msg_buf[1]);
                spi_mastertransmit_nr(msg_buf[2]);
                spi_mastertransmit_nr(msg_buf[3]);
                spi_mastertransmit_nr(msg_buf[4]);

                answerlen =3;
                // msg_buf[0] = CMD_PROGRAM_FUSE_ISP; or CMD_PROGRAM_LOCK_ISP
                msg_buf[1] = STATUS_CMD_OK;
                msg_buf[2] = STATUS_CMD_OK;
                break;

                case CMD_READ_OSCCAL_ISP:
                case CMD_READ_SIGNATURE_ISP:
                case CMD_READ_LOCK_ISP:
                case CMD_READ_FUSE_ISP:  
				for(ci=0;ci<4;ci++) {
					tmp = spi_mastertransmit(msg_buf[ci+2]);
					if ( msg_buf[1] == (ci + 1) ) msg_buf[2] = tmp;
                }
                answerlen = 4;
                //msg_buf[0] = CMD_READ_FUSE_ISP;// or CMD_READ_LOCK_ISP or ...
                msg_buf[1] = STATUS_CMD_OK;
                // msg_buf[2] is the data (fuse byte)
                msg_buf[3] = STATUS_CMD_OK;
                break;

                case CMD_SPI_MULTI:
                // 0: CMD_SPI_MULTI
                // 1: NumTx
                // 2: NumRx 
                // 3: RxStartAddr counting from zero
                // 4+: TxData (len in NumTx)
                // example: 0x1d 0x04 0x04 0x00   0x30 0x00 0x00 0x00
                tmp=msg_buf[2];
                tmp2=msg_buf[3];
                cj=0; 
                ci=0;       
                for (cj=0; cj<msg_buf[1]; cj++) {
                    if (cj >= tmp2 && ci <tmp){
						// store answer starting from msg_buf[2]
						msg_buf[ci+2]=spi_mastertransmit(msg_buf[cj+4]);
						ci++;
                    } else {
						spi_mastertransmit_nr(msg_buf[cj+4]);
                    }
                }
                // padd with zero:
                while(ci<tmp){
					msg_buf[ci+2]=0;
					ci++;
                }
                answerlen = ci+3;
                // msg_buf[0] = CMD_SPI_MULTI
                msg_buf[1] = STATUS_CMD_OK;
                // msg_buf[2...ci+1] is the data 
                msg_buf[ci+2] = STATUS_CMD_OK;
                break;

                default:
                // we should not come here
                answerlen = 2;
                msg_buf[1] = STATUS_CMD_UNKNOWN;
                break;
        }
        transmit_answer(seqnum,answerlen);
        LED3_OFF;
}






int main(void)
{

	//all pins as inputs with pullups
	DDRB  = 0b00000000;
	PORTB = 0b11111111;
	
	DDRC  = 0b00000000;
	PORTC = 0b11111111;
	
	DDRD  = 0b00000000;
	PORTD = 0b11111111;
		
		
		
		spi_init();
		
		ExtClock_Init();
		
		
		unsigned char ch;
        //unsigned char prev_ch=0;
        //unsigned char chr_nl=0;
        unsigned char msgparsestate;
        unsigned char cksum=0;
        unsigned char seqnum=0;
        unsigned int msglen=0;
        unsigned int i=0;
		
		
        // set the clock speed to "no pre-scaler" (8MHz with internal osc or
        // full external speed)
        // set the clock prescaler. First write CLKPCE to enable setting of clock the
        // next four instructions.
        CLKPR=(1<<CLKPCE); // change enable
        CLKPR=(0b0000 & 0x0f); // "no pre-scaler"


		suspendpininit();

        led_init();
		
		       
		adc_init();
		
        ch=0;	
        uart_init();
        sei();
        wd_init();
		
		BootInit();
       
        msgparsestate=MSG_IDLE;
		
		
		LED1_ON;
		LED2_OFF;
		LED3_OFF;
		
		delay_ms(1000);

		
        while(1) {
                
				
				
				ch=uart_getchar();
				
				
                // parse message according to appl. note AVR068 table 3-1:
				if (msgparsestate==MSG_WAIT_CKSUM) {
                        if (ch==cksum && msglen > 0) {
                                // message correct, process it
                                wdt_reset();
                                programcmd(seqnum);

                        } else {
                                msg_buf[0] = ANSWER_CKSUM_ERROR;
                                msg_buf[1] = STATUS_CKSUM_ERROR;
                                transmit_answer(seqnum,2);
                        }
                        // no continue here, set state=MSG_IDLE
                } else if (msgparsestate==MSG_IDLE && ch == MESSAGE_START) {
                        msgparsestate=MSG_WAIT_SEQNUM;
                        cksum = ch^0;
                        continue;
                } else if (msgparsestate==MSG_WAIT_SEQNUM) {
                        seqnum=ch;
                        cksum^=ch;
                        msgparsestate=MSG_WAIT_SIZE1;
                        continue;
                } else if (msgparsestate==MSG_WAIT_SIZE1) {
                        cksum^=ch;
                        msglen=ch<<8;
                        msgparsestate=MSG_WAIT_SIZE2;
                        continue;
                } else if (msgparsestate==MSG_WAIT_SIZE2) {
                        cksum^=ch;
                        msglen|=(ch & 0xff);
                        msgparsestate=MSG_WAIT_TOKEN;
                        continue;
                } else if (msgparsestate==MSG_WAIT_TOKEN) {
                        cksum^=ch;
                        if (ch==TOKEN){
                                msgparsestate=MSG_WAIT_MSG;
                                i=0;
                        }else{
                                msgparsestate=MSG_IDLE;
                        }
                        continue;
                } else if (msgparsestate==MSG_WAIT_MSG && i<msglen && i<=BUF_SIZE) {
                        cksum^=ch;
                        msg_buf[i]=ch;
                        i++;
						wdt_reset();
                        if (i==msglen) {
                                msgparsestate=MSG_WAIT_CKSUM;
                        }
                        continue;
                }
                msgparsestate=MSG_IDLE;
                msglen=0;
                seqnum=0;
		//prev_ch=0;
                i=0;
				
        }
	return(0);
}

