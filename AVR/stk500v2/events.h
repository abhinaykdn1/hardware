
#include "cfg500.h"



#define SUSPEND_PIN_MASK (SUSPEND_PINX & 0x01<<SUSPEND_PIN)
#define BOOT_PIN_MASK (BOOT_PINX & 0x01<<BOOT_PIN)





void suspendpininit(void);
void suspendmode(void);


void BootInit(void);
void IfCallBoot(void);


char GetTargetStat(void);
void targetconnect(void);

