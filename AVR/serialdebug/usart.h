#define bauddivider (F_CPU/(16*baudrate)-1)
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

void printstr(char *string)
{
	while (*string!='\0')
	{
		putchr(*string);
		string++;
	}
}

char chartohex(char c) //only for 0x00 - 0x0F
{
	char ch = c & 0x0F;
	char hx = ((ch<10)?(ch+'0'):(ch+'A'-10));
	return hx;
}

char hextochar(char c)
{
	char ch = (c<='9')?(c-'0'):((c<='F')?(c-'A'+10):((c<='f')?(c-'a'+10):0));
	return ch;
}

void printhex(char c)
{
	char lo = c & 0x0F;
	char hi = (c & 0xF0) >> 4;
	putchr(lo + ((lo<10)?'0':'A'-10));
	putchr(hi + ((hi<10)?'0':'A'-10));
}
