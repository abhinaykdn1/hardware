#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 8000000L
#define baudrate 9600L
#define bauddivider (F_CPU/(16*baudrate)-1)
#define HI(x) ((x)>>8)
#define LO(x) ((x)& 0xFF)

#define LED1 4
#define LED2 5
#define LED_PORT PORTD
#define LED_DDR DDRD

#define PWM_OUT 3
#define PWM_DDR DDRB
#define PWM_PORT PORTB

#define TIMER_CLK_DIV64		    0x03	///< Timer clocked at F_CPU/64
#define TIMER_PRESCALE_MASK		0x07	///< Timer Prescaler Bit-Mask

#define TIMER0_PRESCALER TIMER_CLK_DIV64
#define TIMER0_PRESCALER_VALUE (1<<(3*(TIMER0_PRESCALER-1)))

#define MIN_INTERVAL 1000 //microsecond
#define MAX_INTERVAL 2000 //microsecond
#define CLK_INTERVAL (1000000/(F_CPU/TIMER0_PRESCALER_VALUE)) //microseconds
#define MIN_CLKTIMER (MIN_INTERVAL/CLK_INTERVAL) //timer ticks
#define MAX_CLKTIMER (MAX_INTERVAL/CLK_INTERVAL) //timer ticks
#define MID_CLKTIMER ((MAX_CLKTIMER+MIN_CLKTIMER)/2)

#define BV(bit) (1<<(bit))
#define cbi(reg,bit) reg &= ~(BV(bit))
#define sbi(reg,bit) reg |= (BV(bit))
#define	outb(addr, data) addr = (data)
#define	inb(addr) (addr)

volatile unsigned char pwm;

void initIO(void)
{
	LED_DDR = 1<<LED1 | 1<<LED2;
}

void initUSART(void)
{
	UBRRL = LO(bauddivider);
	UBRRH = HI(bauddivider);
	UCSRA = 0;
	UCSRB = 1<<RXEN|1<<TXEN|1<<RXCIE|0<<TXCIE;
	UCSRC = 1<<URSEL|1<<UCSZ0|1<<UCSZ1;
}

void timer0SetPrescaler(unsigned char prescale)
{
	outb(TCCR0, (inb(TCCR0) & ~TIMER_PRESCALE_MASK) | prescale);
}

void initOC0(void)
{
	sbi(PWM_DDR, PWM_OUT);
	cbi(PWM_PORT, PWM_OUT);

	timer0SetPrescaler(TIMER0_PRESCALER); //start timer0 PWM

	sbi(TCCR0, WGM00);
	sbi(TCCR0, WGM01);

	cbi(TCCR0, COM00);
	sbi(TCCR0, COM01);

	pwm = MID_CLKTIMER;
	OCR0 = pwm;
}

void init(void)
{
	initUSART();
	initIO();
	initOC0();
}

ISR(USART_RXC_vect)
{
switch(UDR)
	{
	case '1': {
		sbi(LED_PORT, LED2);
		break;
		}
	case '0': {
		cbi(LED_PORT, LED2);
		break;
		}
	case 'q': {
		if (pwm<MAX_CLKTIMER-1)
			pwm += 1;
		else 
			pwm = MAX_CLKTIMER;
		OCR0 = pwm;
		break;
		}
	case 'a': {
		if (pwm>MIN_CLKTIMER+1)
			pwm -= 1;
		else
			pwm = MIN_CLKTIMER;
		OCR0 = pwm;
		break;
		}

	case 'w': {
		if (pwm<MAX_CLKTIMER-10)
			pwm += 10;
		else 
			pwm = MAX_CLKTIMER;
		OCR0 = pwm;
		break;
		}
	case 's': {
		if (pwm>MIN_CLKTIMER+10)
			pwm -= 10;
		else
			pwm = MIN_CLKTIMER;
		OCR0 = pwm;
		break;
		}
	case 'e': {
		pwm = MAX_CLKTIMER;
		OCR0 = pwm;
		break;
		}
	case 'd': {
		pwm = MIN_CLKTIMER;
		OCR0 = pwm;
		break;
		}
	case 'r': {
		pwm = MID_CLKTIMER;
		OCR0 = pwm;
		break;
		}
	default: {
		break;
		}
	}
}

int main(void)
{
	unsigned char i = 0;
	unsigned char j = 0;
	init();
	sei();

	while (1)
	{
		i++;
		if (i==64) {
			j++;
		}

		if (j>127) {
		sbi(LED_PORT, LED1);	
		} else {
			cbi(LED_PORT, LED1);	
		}
	}
}
