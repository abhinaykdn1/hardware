#define F_CPU 8000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#define baudrate 19200L
//#define baudrate 38400L
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
#define CMD_BUF 4

uint8_t usart_rx_wp;
uint8_t usart_rx_rp;
char usart_rx_buf[USART_RX_BUF];

uint8_t usart_tx_wp;
uint8_t usart_tx_rp;
char usart_tx_buf[USART_TX_BUF];

uint8_t cmd_buf_p;
char cmd_buf[CMD_BUF];

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

void clearcmdbuf()
{
	cmd_buf_p = 0;
	uint8_t i;
	for (i = 0; i<CMD_BUF; i++) {
		cmd_buf[i] = 0;
	}
}

void executecmd()
{
	if (cmd_buf[0]) {
		switch (cmd_buf[0]) {
			case 'I': {
				char b = 0;
				switch (cmd_buf[1]) {
					case 'A': {
						b = PINA;
						break;
					}
					case 'B': {
						b = PINB;
						break;
					}
					case 'C': {
						b = PINC;
						break;
					}
					case 'D': {
						b = PIND;
						break;
					}
					case 'E': {
						b = DDRA;
						break;
					}
					case 'F': {
						b = DDRB;
						break;
					}
					case 'G': {
						b = DDRC;
						break;
					}
					case 'H': {
						b = DDRD;
						break;
					}
					case 'I': {
						b = PORTA;
						break;
					}
					case 'J': {
						b = PORTB;
						break;
					}
					case 'K': {
						b = PORTC;
						break;
					}
					case 'L': {
						b = PORTD;
						break;
					}
				}
				putchr(chartohex(b>>4));
				putchr(chartohex(b));
				printstr("\n\r");
				break;
			}
			case 'O': {
				char b = hextochar(cmd_buf[3])+(hextochar(cmd_buf[2])<<4);
				switch (cmd_buf[1]) {
					case 'A': {
						PORTA = b;
						break;
					}
					case 'B': {
						PORTB = b;
						break;
					}
					case 'C': {
						PORTC = b;
						break;
					}
					case 'D': {
						PORTD = b;
						break;
					}
					case 'E': {
						DDRA = b;
						break;
					}
					case 'F': {
						DDRB = b;
						break;
					}
					case 'G': {
						DDRC = b;
						break;
					}
					case 'H': {
						DDRD = b;
						break;
					}
				}
				putchr(chartohex(b>>4));
				putchr(chartohex(b));
				printstr("\n\r");
				break;
			}

			default: {
				printstr("ERR:UC\n\r");
				break;
			}
		}
	}
}

void addtocmd(char c)
{
	if ((c == '\n') || (c == '\r')) {
		if (cmd_buf_p>0) {
			executecmd();
		}
		clearcmdbuf();
	} else {
		if (cmd_buf_p<CMD_BUF) {
			char ch = 0;
			if ((c>='a') & (c<='z')) {
				ch = c - 0x20;
			} else if (((c>='0') & (c<='9')) | ((c>='A') & (c<='Z'))) {
				ch = c;
			}
			if (ch != 0) {
				cmd_buf[cmd_buf_p] = ch;
				cmd_buf_p++;
				//putchr(ch);
			}
		}
	}
}

int main(void)
{
	initUSART();
	clearcmdbuf();

	printstr("SD0.01\n\r");
	DDRD |= bv(5);

	while (1)
	{
		cli();
		ibi(PORTD, 5);
		sei();
		if (chrready()) {
			char c = getchr();
			putchr(c);
			addtocmd(c);
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


