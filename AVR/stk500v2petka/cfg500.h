/* vim: set sw=8 ts=8 si et: */
/*
Title:   C include file for configuration of avrusb500 V2
Target:    atmega8/atmega88/atmega168
Copyright: GPL V2
Author: Guido Socher
*/
#ifndef CFG500_H
#define CFG500_H

// remember to change this version string after any update:
#define AVRUSB500VSTR   "avrusb500v2-1.2 \r\n"

#define BAUDRATE	115200

//#define GENERATE_1MHZ

#if F_CPU > 14745600UL
#define VAR_18CLK 1
#else
#define VAR_14CLK 1
#endif




// do not change here! Edit the beginning of the Makefile
#if defined (__AVR_ATmega168__)
#define VAR_88CHIP 1
#elif defined (__AVR_ATmega8__)
#define VAR_8CHIP 1
#elif defined (__AVR_ATmega88__)
#define VAR_88CHIP 1
#elif defined (__AVR_ATmega48__)
#define VAR_88CHIP 1
#endif


#endif /* CFG500_H */
