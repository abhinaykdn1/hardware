#define F_CPU 8000000L
#include <avr/io.h>
#include <avr/interrupt.h>

//#define baudrate 9600L
#define baudrate 38400L
#define bauddivider (F_CPU/(16*baudrate)-1)
#define HI(x) ((x)>>8)
#define LO(x) ((x)& 0xFF)

#define bv(bit) (1<<(bit))
#define cbi(reg,bit) reg &= ~(bv(bit))
#define sbi(reg,bit) reg |= (bv(bit))
#define ibi(reg,bit) reg ^= (bv(bit))
#define outb(addr, data) addr = (data)
#define inb(addr) (addr)

#define USART_TX_BUF 32
#define USART_RX_BUF 32

uint8_t usart_rx_wp;
uint8_t usart_rx_rp;
char usart_rx_buf[USART_RX_BUF];

uint8_t usart_tx_wp;
uint8_t usart_tx_rp;
char usart_tx_buf[USART_TX_BUF];

void initUSART(void)
{
        UBRRL = LO(bauddivider);
        UBRRH = HI(bauddivider);
        UCSRA = 0;
        UCSRB = 1<<RXEN|1<<TXEN|1<<RXCIE|1<<TXCIE;
        UCSRC = 1<<URSEL|1<<UCSZ0|1<<UCSZ1;

		usart_rx_wp = 0;
		usart_rx_rp = 0;

		usart_tx_wp = 0;
		usart_tx_rp = 0;
}

void putchr(char ch)
{
	cli();
	if (usart_tx_wp<USART_TX_BUF) {
		usart_tx_buf[usart_tx_wp] = ch;
		usart_tx_wp++;
	} //else buffer overflow error
	sei();

	if (UCSRA & (1<<UDRE)) {
		UDR = usart_tx_buf[usart_tx_rp];
		usart_tx_rp++;
		if (usart_tx_rp>=usart_tx_wp) {
			usart_tx_rp = 0;
			usart_tx_wp = 0;
		}
	}
}

char chrready(void)
{
	return (usart_rx_wp>usart_rx_rp);
}

char getchr(void)
{
	sei();
	while (usart_rx_wp<=usart_rx_rp);
//	{
//		char a = 1;
//		a++;
//	} //wait for char
	cli();
	char c = usart_rx_buf[usart_rx_rp];
	usart_rx_rp++;
	if (usart_rx_rp >= usart_rx_wp) {
		usart_rx_rp = 0;
		usart_rx_wp = 0;
	}
	sei();
	return c;
}

void printstr(char *string)
{
	while (*string!='\0')
	{
		putchr(*string);
		string++;
	}
}

int main(void)
{
	//c = 0;
	initUSART();

	printstr("Hello world!\n\r");
//	print();

//	UDR = c;
	DDRD = bv(5);

	while (1)
	{
		cli();
		ibi(PORTD, 5);
		sei();
		if (chrready()) {
			char c = getchr();
			putchr(c);
		}
	}
}


ISR(USART_RXC_vect)
{
	char c = UDR;
	if (usart_rx_wp<USART_RX_BUF) {
		usart_rx_buf[usart_rx_wp] = c;
		usart_rx_wp++;
	} //else we lose byte from UART
}

ISR(USART_TXC_vect)
{
	if (usart_tx_rp<usart_tx_wp) {
		UDR = usart_tx_buf[usart_tx_rp];
		usart_tx_rp++;
		if (usart_tx_rp>=usart_tx_wp) {
			usart_tx_rp = 0;
			usart_tx_wp = 0;
		}
	}	
}


