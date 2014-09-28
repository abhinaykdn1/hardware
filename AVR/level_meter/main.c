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

void lcd_printhex(uint8_t c)
{
	char lo = c & 0x0F;
	char hi = (c & 0xF0) >> 4;
	lcd_putchar(hi + ((hi<10)?'0':'A'-10));
	lcd_putchar(lo + ((lo<10)?'0':'A'-10));
}

void lcd_printdec(uint16_t pc) {
	uint16_t c = pc;
	char *n = "              ";
	uint8_t i = 4;
	n[i--] = (c % 10) + '0';
	while (c) {
		c = c / 10;
		n[i--] = (c % 10) + '0';
	}
	lcd_putstr(n);
}

int main(void)
{
	init();

//	lcd_overlay(1);
	lcd_optimize(0);
	lcd_rotate(1);
	lcd_clear(0x00);

//	uint8_t x = 0;
	uint32_t avg_dist = 0;
	while (1) {
//		lcd_clear(0x00);
		lcd_textpos(0,0);
		uint16_t dist = getDistance();
		avg_dist = (dist + 7*avg_dist)>>3;
//		lcd_printhex((dist>>8)&0x00FF);
//		lcd_printhex(dist&0x00FF);
		lcd_printdec(dist);
		lcd_printdec(avg_dist);
//		uint32_t dist_cm = dist/116;
//		uint16_t dist_cm = dist/29;
//		lcd_printdec(dist_cm);
//		uint32_t vol_l = ((dist*10)/231);
//		uint16_t vol_l = dist/4;
//		lcd_printdec(vol_l);

//		uint8_t dst = (dist>>7)%LCD_HEIGHT;
//		lcd_overlay(0);
//		lcd_line(x, LCD_HEIGHT-1, x, 0);
//		lcd_overlay(1);
//		lcd_line(x, LCD_HEIGHT-1, x, 0);
//		lcd_overlay(0);
//		lcd_line(x, LCD_HEIGHT-1, x, dst);
//		lcd_send_buffer();
//
//		x++;
//		if (x>LCD_WIDTH) {
//			x = 0;
//		}
	}


	return 0;
}

#ifdef __AVR_ATmega32U4__
ISR(USB_COM_vect) {
}

ISR(USB_GEN_vect) {
}
#endif
