#include <avr/io.h>

#define LED0 6
#define LED0_PORT PORTB
#define LED0_DDR DDRB

#define LED1 7
#define LED1_PORT PORTB
#define LED1_DDR DDRB
 
#define LED2 5
#define LED2_PORT PORTD
#define LED2_DDR DDRD

#define LED3 6
#define LED3_PORT PORTD
#define LED3_DDR DDRD

#define PWRC 7
#define PWRC_PORT PORTD
#define PWRC_DDR DDRD

#define BV(bit) (1<<(bit))
#define cbi(reg,bit) reg &= ~(BV(bit))
#define sbi(reg,bit) reg |= (BV(bit))

#define ADC33V 4
#define ADC5V 5
#define ADC12V 3
#define ADC1S 2
#define ADC2S 1
#define ADC3S 0

int main(void)
{
	sbi(LED0_DDR, LED0);
	sbi(LED1_DDR, LED1);
	sbi(LED2_DDR, LED2);
	sbi(LED3_DDR, LED3);

	sbi(PWRC_DDR, PWRC);

	unsigned int i = 0;
	unsigned int j = 0;

	while (1)
	{
		i++;
		if (i%2000==0) {
			j++;
		}

		switch (j%4) {
			case 0: {
				sbi(LED0_PORT, LED0);
				cbi(LED1_PORT, LED1);
				cbi(LED2_PORT, LED2);
				cbi(LED3_PORT, LED3);
				break;
			}
			case 1: {
				cbi(LED0_PORT, LED0);
				sbi(LED1_PORT, LED1);
				cbi(LED2_PORT, LED2);
				cbi(LED3_PORT, LED3);
				break;
			}
			case 2: {
				cbi(LED0_PORT, LED0);
				cbi(LED1_PORT, LED1);
				sbi(LED2_PORT, LED2);
				cbi(LED3_PORT, LED3);
				break;
			}
			case 3: {
				cbi(LED0_PORT, LED0);
				cbi(LED1_PORT, LED1);
				cbi(LED2_PORT, LED2);
				sbi(LED3_PORT, LED3);
				break;
			}
		}
		
		if ((j%32)>15) {
			sbi(PWRC_PORT, PWRC);
		} else {
			cbi(PWRC_PORT, PWRC);
		}
			
	}
}
