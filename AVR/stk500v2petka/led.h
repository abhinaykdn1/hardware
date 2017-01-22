/* vim: set sw=8 ts=8 si et: */
#ifndef LED_H
#define LED_H
#include <avr/io.h>
#include "gpio.h"

// LED
#define MY_LED_PORT		B
#define MY_LED_PIN		2 
#define MY_LED_ALEV		H 


// enable PB1 as output
#define LED_INIT dir_out(MY_LED)
// led on
#define LED_ON bit_on(MY_LED)
// led off 
#define LED_OFF bit_off(MY_LED)
// 
#define LED_IS_OFF (!(PORTB & (1<<MY_LED_PIN))) // TODO remove this!

#endif //LED_H
