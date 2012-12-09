#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 8000000L
#define F_TARGET 40000
#define F_INT 10

#define OUT1 7
#define OUT2 6
#define OUT3 5
#define OUT4 4
#define OUT_PORT PORTD
#define OUT_DDR DDRD

#define bv(bit) (1<<(bit))
#define cbi(reg,bit) reg &= ~(bv(bit))
#define sbi(reg,bit) reg |= (bv(bit))
#define ibi(reg,bit) reg ^= (bv(bit))
#define outb(addr, data) addr = (data)
#define inb(addr) (addr)

#define TIMER_CLK_DIV1              0x01        ///< Timer clocked at F_CPU
#define TIMER_CLK_DIV8              0x02        ///< Timer clocked at F_CPU/8
#define TIMER_CLK_DIV64             0x03        ///< Timer clocked at F_CPU/64
#define TIMER_PRESCALE_MASK         0x07        ///< Timer Prescaler Bit-Mask
 
#define TIMER2_PRESCALER TIMER_CLK_DIV1
#define TIMER2_PRESCALER_VALUE (1<<(3*(TIMER2_PRESCALER-1)))

#define OCR2_VALUE (((F_CPU / TIMER2_PRESCALER_VALUE) / F_TARGET) / 2 - 1)

volatile unsigned int time0;

void initIO(void)
{
    OUT_DDR = bv(OUT1) | bv(OUT2) | bv(OUT3) | bv(OUT4);
}
 
/*
void initUSART(void)
{
        UBRRL = LO(bauddivider);
        UBRRH = HI(bauddivider);
        UCSRA = 0;
        UCSRB = 1<<RXEN|1<<TXEN|1<<RXCIE|0<<TXCIE;
        UCSRC = 1<<URSEL|1<<UCSZ0|1<<UCSZ1;
}
*/

void timer2SetPrescaler(unsigned char prescale)
{
	outb(TCCR2, (inb(TCCR2) & ~TIMER_PRESCALE_MASK) | prescale);
}
 
void initOC2(void)
{
	sbi(TIMSK, OCIE2); //Interrupt on compare
	//sbi(TIMSK, TOIE0); //Interrupt on overflow

    timer2SetPrescaler(TIMER2_PRESCALER); //start timer2 PWM
 
//    cbi(TCCR0, WGM00);
//    cbi(TCCR0, WGM01); //Normal waveform generation mode
    cbi(TCCR2, WGM20);
    sbi(TCCR2, WGM21); //CTC (clear on compare) mode

    cbi(TCCR2, COM20);
    cbi(TCCR2, COM21); //Normal port operation, OC0 disconnected


	sei();               // Enable interrupts
 
    OCR2 = OCR2_VALUE;
}

void init(void)
{
//        initUSART();
        initIO();
        initOC2();
		sbi(OUT_PORT, OUT3);
		cbi(OUT_PORT, OUT2);
		sbi(OUT_PORT, OUT4);
		cbi(OUT_PORT, OUT1);
		time0 = 0;
}

int main(void)
{

	init();

	while (1)
	{
	//	cli();
	//	ibi(OUT_PORT, OUT1);
	//	sei();
	}
}

/*
ISR (TIMER0_OVF_vect)
{
	tt0++;
	if (tt0%2==1) {
		sbi(OUT_PORT, OUT2);
	} else {
		cbi(OUT_PORT, OUT2);	
	}

}
*/

ISR (TIMER2_COMP_vect)
{
	OUT_PORT ^= bv(OUT4) | bv(OUT1);
	if (time0<40) {
		OUT_PORT ^= bv(OUT2) | bv(OUT3);
	}
	time0++;

	if (time0>400) {
		time0 = 0;
	}
}
