/* vim: set sw=8 ts=8 si : */
/*
Title	:   C include file for analog conversion
Target:    atmega8
Copyright: GPL V2
Autor: Guido Socher
*/
#ifndef ANALOG_H
#define ANALOG_H
#define RESETADC 0

// return analog value of a given channel.
extern unsigned int convertanalog(unsigned char channel);
extern unsigned char analog2v(unsigned int aval);
extern unsigned char reset_works(void);
extern void config_reset_checkstate(unsigned char p);
extern unsigned char config_get_reset_checkstate(void);
extern unsigned char prg_state_get(void);
extern void prg_state_set(unsigned char p);

#endif /* ANALOG_H */
