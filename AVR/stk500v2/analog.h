/* vim: set sw=8 ts=8 si : */
/*
Title	:   C include file for analog conversion
Target:    atmega8
Copyright: GPL V2
Autor: Guido Socher
*/


#define MINVOLTAGE 16 // Min. voltage from target (16 = 1.6v)
#define HYSTERESIS 01 // Hysteresis (03 = 0.3v)


// return analog value of a given channel.
extern void adc_init(void);
extern unsigned int convertanalog(void);
extern unsigned char analog2v(unsigned int aval);
extern unsigned char prg_state_get(void);
extern void prg_state_set(unsigned char p);


