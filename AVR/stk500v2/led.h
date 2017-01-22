/* vim: set sw=8 ts=8 si et: */

#include <avr/io.h>
#include "cfg500.h"




#define LED1_ON {LED1_PORT |= (1<<LED1_PIN);}
#define LED1_OFF {LED1_PORT &=~ (1<<LED1_PIN);}
 
#define LED2_ON {LED2_PORT |= (1<<LED2_PIN);}
#define LED2_OFF {LED2_PORT &=~ (1<<LED2_PIN);}

#define LED3_ON {LED3_PORT |= (1<<LED3_PIN);}
#define LED3_OFF {LED3_PORT &=~ (1<<LED3_PIN);}




void led_init(void);
