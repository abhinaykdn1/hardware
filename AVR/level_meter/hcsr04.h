/*
 * hcsr04.h
 *
 *  Created on: Sep 27, 2014
 *      Author: lim
 */

#ifndef HCSR04_H_
#define HCSR04_H_

#include "ports.h"

#define HCSR04_TRIG PORTA_D2
#define HCSR04_ECHO PORTA_D10

void initHCSR04(void);
uint16_t getDistance();

#endif /* HCSR04_H_ */
