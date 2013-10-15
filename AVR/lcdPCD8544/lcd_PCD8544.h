#include "ports.h"
#include <avr/pgmspace.h>

#define LCD_RST PORT_D7
#define LCD_CE  PORT_D6
#define LCD_DC  PORT_D5
#define LCD_DIN PORT_D3
#define LCD_CLK PORT_D2

#define LCD_LED PORT_B0

#define LCD_WIDTH 84
#define LCD_HEIGHT 48
#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8
#define LCD_LINES (LCD_HEIGHT / CHAR_HEIGHT)
#define LCD_COLS (LCD_WIDTH / CHAR_WIDTH)
#define LCD_CHARS (LCD_LINES*LCD_COLS)
#define LCD_BYTES (LCD_WIDTH * LCD_HEIGHT / 8)

void lcd_clear(void);
void lcd_fill(void);
void lcd_str(char *str);
void lcd_init(void);
