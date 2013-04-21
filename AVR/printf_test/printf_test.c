#define F_CPU 1600000L

#include <avr/io.h>
#include <stdio.h>

#define BAUD 9600L
/*
#define bauddivider (F_CPU/(16*baudrate)-1)
*/
#define HI(x) ((x)>>8)
#define LO(x) ((x)& 0xFF)
#define UBRR_VALUE (F_CPU/(16*BAUD)-1)
#define UBRRL_VALUE LO(UBRR_VALUE)
#define UBRRH_VALUE HI(UBRR_VALUE)

//#include <util/setbaud.h>

int uart_putchar(char c, FILE *stream)
{
	if (c == '\n') {
		uart_putchar('\r', stream);
	}
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;
	return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void init_uart()
{
    UBRRL = UBRRL_VALUE;
    UBRRH = UBRRH_VALUE;
    UCSRA = 0;
    UCSRB = 1<<RXEN|1<<TXEN/*|1<<RXCIE|1<<TXCIE*/;
    UCSRC = 1<<URSEL|1<<UCSZ0|1<<UCSZ1;
	stdout = &mystdout;
}

void init()
{
	init_uart();
}

int main(void)
{
	init();	

	printf("Hello\n");
	int i = 0;
	while(1)
	{
		uart_putchar('1', NULL);
//		printf("%i\n", i);
		i++;
	}
}
