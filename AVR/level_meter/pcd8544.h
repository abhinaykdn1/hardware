/*
 * pcd8544.h
 *
 *  Created on: Sep 26, 2014
 *      Author: lim
 */

#ifndef PCD8544_H_
#define PCD8544_H_

#include "ports.h"
#include <avr/pgmspace.h>

#define LCD_RST PORTA_D8
#define LCD_CE  PORTA_D7
#define LCD_DC  PORTA_D6
#define LCD_DIN PORTA_D5
#define LCD_CLK PORTA_D4
#define LCD_LED PORTA_D3

#define LCD_WIDTH 84
#define LCD_HEIGHT 48
#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8
#define LCD_LINES (LCD_HEIGHT / CHAR_HEIGHT)	//6
#define LCD_COLS (LCD_WIDTH / CHAR_WIDTH)		//14
#define LCD_CHARS (LCD_LINES*LCD_COLS)			//84
#define LCD_BYTES (LCD_WIDTH * LCD_HEIGHT / 8)	//504

void lcd_init(void);
void lcd_clear(uint8_t pattern);
void lcd_putstr(char *str);
void lcd_overlay(uint8_t overlay);
void lcd_rotate(uint8_t rotate);
void lcd_optimize(uint8_t optimize);
void lcd_point(uint8_t x, uint8_t y);
void lcd_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void lcd_circle(uint8_t x0, uint8_t y0, uint8_t r);
void lcd_send_buffer(void);

#endif /* PCD8544_H_ */
