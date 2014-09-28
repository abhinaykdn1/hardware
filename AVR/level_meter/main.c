#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "ports.h"
#include "pcd8544.h"
#include "hcsr04.h"
#include "timer1.h"

void init(void) {
	SETD(TXLED);
	SETD(RXLED);

#ifdef __AVR_ATmega32U4__
	USBCON = (0<<USBE)|(1<<FRZCLK)|(0<<OTGPADE)|(0<<VBUSTE);
#endif
	initTimer1();
	initHCSR04();
	lcd_init();
	sei();
}

int main(void)
{
	init();

//	lcd_overlay(1);
	lcd_optimize(0);
	lcd_rotate(1);
	lcd_clear(0x00);
	lcd_putstr("Muwa u Mawa");

	while (1) {
		uint16_t dist = getDistance();
		lcd_overlay(1);
		for (uint8_t x=0; x<LCD_WIDTH; x++) {
			for (uint8_t y=0; y<LCD_HEIGHT; y++) {
				lcd_point(x,y);
//				lcd_send_buffer();
			}
			lcd_send_buffer();
		}
//		lcd_send_buffer();
		for (uint8_t x=0; x<LCD_WIDTH; x+=2) {
			lcd_line(0,0,x,LCD_HEIGHT-1);
			lcd_line(LCD_WIDTH-1,0,x,LCD_HEIGHT-1);
			lcd_line(x,0,LCD_WIDTH-1,LCD_HEIGHT-1);
			lcd_line(x,0,0,LCD_HEIGHT-1);
			lcd_send_buffer();
		}
//		lcd_send_buffer();

		for (uint8_t r=0; r<LCD_HEIGHT; r+=2) {
			lcd_circle(LCD_WIDTH/2,LCD_HEIGHT/2,r);
			lcd_send_buffer();
		}
//		lcd_send_buffer();

		lcd_overlay(0);
	}


	return 0;
}

#ifdef __AVR_ATmega32U4__
ISR(USB_COM_vect) {
}

ISR(USB_GEN_vect) {
}
#endif
