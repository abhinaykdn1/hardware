#include <avr/io.h>

#define LED1 4
#define LED_PORT PORTD
#define LED_DDR DDRD

#define BV(bit) (1<<(bit))
#define cbi(reg,bit) reg &= ~(BV(bit))
#define sbi(reg,bit) reg |= (BV(bit))

int main(void)
{
	unsigned char i = 0;

	while (1)
	{
		i++;
		if (i%2==1) {
			sbi(LED_PORT, LED1);	
		} else {
			cbi(LED_PORT, LED1);	
		}
	}
}
