/*
 * main.c
 *
 *  Created on: Sep 16, 2013
 *      Author: lim
 */
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "ports.h"
#include "lcd_PCD8544.h"

#define LED PORT_C5

void lcd_string_format(char *szFormat, ...)
{
	char szBuffer[256]; //in this buffer we form the message
	int NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
	int LASTCHAR = NUMCHARS - 1;
	va_list pArgs;
	va_start(pArgs, szFormat);
	vsnprintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
	va_end(pArgs);
	lcd_str(szBuffer);
}

int main(void)
{
	lcd_init();

	SETD(LED);
	SETP(LED);

	SETP(LCD_LED);
	SETD(LCD_LED);

//	for (unsigned char i = 0; i < 200; ++i) {
//		lcd_rand();
//	}
	lcd_fill();
	_delay_ms(100);
	lcd_clear();
	lcd_clear();

	lcd_str("Hello world!\n\0");

	_delay_ms(1000);

	int i = 0;
//	float k = 0.3;
	while (1) {
//		k = 0.3*k*k + 2*k - 1;
		long l = i*i;
		lcd_string_format("%x %x \n", i, l);
//		i += F_CPU;
//		_delay_ms(50);
		i++;
//		i &= 0x07f;
	}

	return 0;
}

