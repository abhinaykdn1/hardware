#define F_CPU 14745600L
#define BAUDRATE 57600L

#define LED0 PORTD4
#define LED1 PORTD5
#define LED_PORTD_MASK (bv(LED0)|bv(LED1))

#define IN0 PORTB0
#define IN1 PORTB1
#define IN2 PORTB2
#define IN3 PORTB3
#define IN4 PORTB4
#define IN5 PORTB5
#define IN6 PORTD6
#define IN7 PORTD7
#define IN_PWM_COUNT 8
#define IN_PORTB_MASK (bv(IN0)|bv(IN1)|bv(IN2)|bv(IN3)|bv(IN4)|bv(IN5))
#define IN_PORTD_MASK (bv(IN6)|bv(IN7))

#define OUT0 PORTC0
#define OUT1 PORTC1
#define OUT2 PORTC2
#define OUT3 PORTC3
#define OUT4 PORTC4
#define OUT5 PORTC5
#define OUT6 PORTD2
#define OUT7 PORTD3
#define OUT_PWM_COUNT 8
#define OUT_PORTC_MASK (bv(OUT0)|bv(OUT1)|bv(OUT2)|bv(OUT3)|bv(OUT4)|bv(OUT5))
#define OUT_PORTD_MASK (bv(OUT6)|bv(OUT7))

#define TIMER0A_VALUE 147
