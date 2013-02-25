#define USART_TX_BUF 32
#define USART_RX_BUF 32
#define CMD_BUF 4
#define BAUDDIVIDER (F_CPU/(16*BAUDRATE)-1)

#include <avr/interrupt.h>

uint8_t usart_rx_wp;
uint8_t usart_rx_rp;
char usart_rx_buf[USART_RX_BUF];

uint8_t usart_tx_wp;
uint8_t usart_tx_rp;
char usart_tx_buf[USART_TX_BUF];

uint8_t cmd_buf_p;
char cmd_buf[CMD_BUF];

void inituart(void)
{
	UBRR0L = LO(BAUDDIVIDER);
	UBRR0H = HI(BAUDDIVIDER);
	UCSR0B = 1<<RXEN0|1<<TXEN0|1<<RXCIE0|1<<TXCIE0;
	UCSR0C = 1<<UCSZ00|1<<UCSZ01;


	usart_rx_wp = 0;
	usart_rx_rp = 0;

	usart_tx_wp = 0;
	usart_tx_rp = 0;
}

ISR(USART_RX_vect)
{
	char c = UDR0;
	if (usart_rx_wp<USART_RX_BUF) {
		usart_rx_buf[usart_rx_wp] = c;
		usart_rx_wp++;
	} //else we lose byte from UART
}

ISR(USART_TX_vect)
{
	if (usart_tx_rp<usart_tx_wp) {
		UDR0 = usart_tx_buf[usart_tx_rp];
		usart_tx_rp++;
		if (usart_tx_rp>=usart_tx_wp) {
			usart_tx_rp = 0;
			usart_tx_wp = 0;
		}
	}	
}

void putchr(char ch)
{
	cli();
	if (usart_tx_wp<USART_TX_BUF) {
		usart_tx_buf[usart_tx_wp] = ch;
		usart_tx_wp++;
	} //else buffer overflow error
	sei();

	if (UCSR0A & (1<<UDRE0)) {
		UDR0 = usart_tx_buf[usart_tx_rp];
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
	while (!chrready());
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

