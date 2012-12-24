#define TIMER_CLK_DIV1              0x01        ///< Timer clocked at F_CPU
#define TIMER_CLK_DIV8              0x02        ///< Timer clocked at F_CPU/8
#define TIMER_CLK_DIV64             0x03        ///< Timer clocked at F_CPU/64
#define TIMER_PRESCALE_MASK         0x07        ///< Timer Prescaler Bit-Mask

#define TIMER2_PRESCALER TIMER_CLK_DIV1
#define TIMER2_PRESCALER_VALUE (1<<(3*(TIMER2_PRESCALER-1)))
#define OCR2_VALUE (((F_CPU / TIMER2_PRESCALER_VALUE) / F_TARGET) / 2 - 1)

void timer2SetPrescaler(unsigned char prescale)
{
	outb(TCCR2A, (inb(TCCR2A) & ~TIMER_PRESCALE_MASK) | prescale);
}
 
void initOC2(void)
{
	sbi(TIMSK2, OCIE2A); //Interrupt on compare
	//sbi(TIMSK, TOIE0); //Interrupt on overflow

    timer2SetPrescaler(TIMER2_PRESCALER); //start timer2 PWM
 
//    cbi(TCCR0, WGM00);
//    cbi(TCCR0, WGM01); //Normal waveform generation mode
    cbi(TCCR2A, WGM20);
    sbi(TCCR2A, WGM21); //CTC (clear on compare) mode

    cbi(TCCR2A, COM2A0);
    cbi(TCCR2A, COM2A1); //Normal port operation, OC0 disconnected


	sei();               // Enable interrupts
 
    OCR2A = OCR2_VALUE;
}


