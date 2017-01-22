/* vim: set sw=8 ts=8 si et: */
/*************************************************************************
 Title:   C include file
 Copyright: GPL
***************************************************************************/
#ifndef TOUT_H
#define TOUT_H
#include <avr/wdt.h>

extern void delay_ms(unsigned int ms);
extern void wd_init(void);
//
#define wd_kick() wdt_reset()


#endif /* TOUT_H */
