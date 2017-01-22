
#include <avr/io.h>
#include "cfg500.h"
#include "events.h"
#include "led.h"
#include "timeout.h"
#include "spi.h"
#include "analog.h"
#include "exclock.h"


static void (*jmp_to_boot)(void) = (void(*))0x0C00; //jmp_to_boot(); - перейти в секцию загрузчика

char targetstatus = 0;

char last1=1, last2=0;

//SUSPEND

void suspendpininit(void) {
	SUSPEND_DDR  &= ~(1<<SUSPEND_PIN);
	SUSPEND_PORT &= ~(1<<SUSPEND_PIN);
}


void suspendmode(void) {

	
	while(SUSPEND_PIN_MASK) { //Suspend mode
		wdt_reset();
		
		if(last1) {
			last1=0; last2=1;
			
			//entering suspend mode
			LED2_OFF;
			if (prg_state_get()) spi_stop();
			EXCLOCK_BUFF_OFF;
		}
		
			//loop
			LED1_OFF;
			delay_ms(600);
			LED1_ON;
			if (!SUSPEND_PIN_MASK) break;
			delay_ms(600);
		
		
	}
	
	
	
	
	if(last2) {
		last1=1; last2=0;
		
		//leaving suspend mode
		if(get_osc_pscale()) EXCLOCK_BUFF_ON;
	}
	

}











//Enter to BOOTloader

void BootInit(void) {
	BOOT_DDR &= ~(1<<BOOT_PIN);
	BOOT_PORT |= (1<<BOOT_PIN);
	
	//PORTD3 - output(0)
	DDRD |= (1<<PD3);
	PORTD &= ~(1<<PD3);
	
}






//start bootloader
void IfCallBoot(void) {
	if(!BOOT_PIN_MASK) {
	//----------------------------------
		
		//Включаем все светодиоды
		LED1_ON;
		LED2_ON;
		LED3_ON;
		
		//Отключаем все буферы
		BUFFS_OFF;
		EXCLOCK_BUFF_OFF;
		
		jmp_to_boot(); //Переходим к загрузчику
		
	//----------------------------------
	}
}








//CONNECT


char GetTargetStat(void) {
	return targetstatus;
}

void targetconnect(void) {

	//включаем светодиод, если подключен 
	if (convertanalog() < MINVOLTAGE - HYSTERESIS) {
		targetstatus = 0;
		
		LED2_OFF;
		EXCLOCK_BUFF_OFF;
	} else if (convertanalog() >= MINVOLTAGE) {
		targetstatus = 1;
		
		LED2_ON;
		if(get_osc_pscale()) EXCLOCK_BUFF_ON;
	}
			
}












