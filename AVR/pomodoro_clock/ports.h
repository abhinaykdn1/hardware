#define PORT_A0 PORTA,DDRA,PINA,PA0
#define PORT_A1 PORTA,DDRA,PINA,PA1
#define PORT_A2 PORTA,DDRA,PINA,PA2
#define PORT_A3 PORTA,DDRA,PINA,PA3
#define PORT_A4 PORTA,DDRA,PINA,PA4
#define PORT_A5 PORTA,DDRA,PINA,PA5
#define PORT_A6 PORTA,DDRA,PINA,PA6
#define PORT_A7 PORTA,DDRA,PINA,PA7

#define PORT_B0 PORTB,DDRB,PINB,PB0
#define PORT_B1 PORTB,DDRB,PINB,PB1
#define PORT_B2 PORTB,DDRB,PINB,PB2
#define PORT_B3 PORTB,DDRB,PINB,PB3
#define PORT_B4 PORTB,DDRB,PINB,PB4
#define PORT_B5 PORTB,DDRB,PINB,PB5
#define PORT_B6 PORTB,DDRB,PINB,PB6
#define PORT_B7 PORTB,DDRB,PINB,PB7

#define PORT_C0 PORTC,DDRC,PINC,PC0
#define PORT_C1 PORTC,DDRC,PINC,PC1
#define PORT_C2 PORTC,DDRC,PINC,PC2
#define PORT_C3 PORTC,DDRC,PINC,PC3
#define PORT_C4 PORTC,DDRC,PINC,PC4
#define PORT_C5 PORTC,DDRC,PINC,PC5
#define PORT_C6 PORTC,DDRC,PINC,PC6
#define PORT_C7 PORTC,DDRC,PINC,PC7

#define PORT_D0 PORTD,DDRD,PIND,PD0
#define PORT_D1 PORTD,DDRD,PIND,PD1
#define PORT_D2 PORTD,DDRD,PIND,PD2
#define PORT_D3 PORTD,DDRD,PIND,PD3
#define PORT_D4 PORTD,DDRD,PIND,PD4
#define PORT_D5 PORTD,DDRD,PIND,PD5
#define PORT_D6 PORTD,DDRD,PIND,PD6
#define PORT_D7 PORTD,DDRD,PIND,PD7

//Arduino pin mapping
#define	PORT_A_D0 PORT_D0
#define	PORT_A_D1 PORT_D1
#define	PORT_A_D2 PORT_D2
#define	PORT_A_D3 PORT_D3
#define	PORT_A_D4 PORT_D4
#define	PORT_A_D5 PORT_D5
#define	PORT_A_D6 PORT_D6
#define	PORT_A_D7 PORT_D7
#define	PORT_A_D8 PORT_B0
#define	PORT_A_D9 PORT_B1
#define	PORT_A_D10 PORT_B2
#define	PORT_A_D11 PORT_B3
#define	PORT_A_D12 PORT_B4
#define	PORT_A_D13 PORT_B5
#define	PORT_A_A0 PORT_C0
#define	PORT_A_A1 PORT_C1
#define	PORT_A_A2 PORT_C2
#define	PORT_A_A3 PORT_C3
#define	PORT_A_A4 PORT_C4
#define	PORT_A_A5 PORT_C5

#define SETPM(port, ddr, pin, portnum) port = (port | (1<<portnum))
#define CLRPM(port, ddr, pin, portnum) port = (port & (~(1<<portnum)))
#define INVPM(port, ddr, pin, portnum) port = (port ^ (1<<portnum))
#define SETDM(port, ddr, pin, portnum) ddr = (ddr | (1<<portnum))
#define CLRDM(port, ddr, pin, portnum) ddr = (ddr & (~(1<<portnum)))
#define PINVM(port, ddr, pin, portnum) ((pin & (1<<portnum)) != 0)
#define PORTM(port, ddr, pin, portnum) port
#define DDRM(port, ddr, pin, portnum) ddr
#define PINM(port, ddr, pin, portnum) pin
#define PORTNM(port, ddr, pin, portnum) portnum

#define SETP(arg) SETPM(arg)
#define CLRP(arg) CLRPM(arg)
#define INVP(arg) INVPM(arg)
#define SETD(arg) SETDM(arg)
#define CLRD(arg) CLRDM(arg)
#define PINV(arg) PINVM(arg)
#define PORT(arg) PORTM(arg)
#define DDR(arg) DDRM(arg)
#define PIN(arg) PINM(arg)
#define PORTN(arg) PORTNM(arg)
