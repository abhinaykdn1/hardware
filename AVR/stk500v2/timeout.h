/* vim: set sw=8 ts=8 si et: */
/*************************************************************************
 Title:   C include file
 Copyright: GPL
***************************************************************************/
#ifndef TOUT_H
#define TOUT_H
#include <avr/wdt.h>
#ifdef VAR_18CLK
#define F_CPU 18432000UL  // 18 MHz
#else
#define F_CPU 14745600UL  // 14 MHz
#endif

extern void delay_ms(unsigned int ms);
extern void wd_init(void);



#endif /* TOUT_H */
