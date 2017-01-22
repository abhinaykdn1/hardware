/* vim: set sw=8 ts=8 si et: */
/*************************************************************************
 Title:   C include file for uart
 Target:    atmega8
 Copyright: GPL
***************************************************************************/
#ifndef UART_H
#define UART_H
#include <avr/pgmspace.h>

extern void uart_init(void);
extern void uart_sendchar(char c);
extern unsigned char uart_getchar(void);





#endif /* UART_H */
