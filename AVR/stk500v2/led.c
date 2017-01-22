

#include <avr/io.h>
#include "led.h"
#include "cfg500.h"



void led_init(void) {
	LED1_DDR |= (1<<LED1_PIN);
	LED1_PORT &= ~(1<<LED1_PIN);
	
	LED2_DDR |= (1<<LED2_PIN);
	LED2_PORT &= ~(1<<LED2_PIN);
	
	LED3_DDR |= (1<<LED3_PIN);
	LED3_PORT &= ~(1<<LED3_PIN);
}
