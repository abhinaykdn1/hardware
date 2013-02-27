.include "m48def.inc"

	.equ	F_CPU = 14745600
	.equ	BAUDRATE = 57600
	.equ	BAUDDIVIDER = F_CPU/(16*BAUDRATE)-1
	.equ	TIMER0A_VAL = 147
	.equ	PWM_CYCLE = 512

;	.equ	PWMO0	R2
;	.equ	PWMO1	R3
;	.equ	PWMO2	R4
;	.equ	PWMO3	R5
;	.equ	PWMO4	R6
;	.equ	PWMO5	R7
;	.equ	PWMO6	R8
;	.equ	PWMO7	R9

;	.equ	PWMI0	R10
;	.equ	PWMI1	R11
;	.equ	PWMI2	R12
;	.equ	PWMI3	R13
;	.equ	PWMI4	R14
;	.equ	PWMI5	R15
;	.equ	PWMI6	R16
;	.equ	PWMI7	R17

;	LASTPORTC R18
;	LASTPORTB R19

.DSEG

	.equ	uart_rx_buf_size = 16
	.equ	uart_tx_buf_size = 16
uart_rx_buf:
	.byte	uart_rx_buf_size
uart_rx_wp:
	.byte	1
uart_rx_rp:
	.byte	1
uart_tx_buf:
	.byte	uart_tx_buf_size
uart_tx_wp:
	.byte	1
uart_tx_rp:
	.byte	1

pwm_out:
	.byte	8

pwm_in:
	.byte	8

.CSEG

.ORG 0x0000
	RJMP	Reset

.include "intvectors.asm"

Reset:
	LDI		R28,Low(RAMEND)     // Stack init
	LDI		R29,High(RAMEND)	// Y register as accumulator 
	OUT		SPH,R29
	OUT		SPL,R28
	CLR		R1            		// Clear register, R1 always zero

Init:
	LDI		R26,Low(PWM_CYCLE)	// X register as timer
	LDI		R27,High(PWM_CYCLE)	// 200h for one PWM cycle


InitUART:
	LDI		R28,Low(BAUDDIVIDER)
	LDI		R29,High(BAUDDIVIDER)
	STS		UBRR0L,R28
	STS		UBRR0H,R29
	LDI		R28,(1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0)
	STS		UCSR0B,R28
	LDI		R28,(1<<UCSZ00)|(1<<UCSZ01)
	STS		UCSR0C,R28

	STS		uart_rx_wp,R1		// Reset to zero buffer pointers
	STS		uart_rx_rp,R1
	STS		uart_tx_wp,R1
	STS		uart_tx_rp,R1

InitTimers:
InitTimer0:
	LDS		R28,TIMSK0
	ORI		R28,(1<<OCIE0A)
	STS		TIMSK0,R28
	
	IN		R28,TCCR0A
	ORI		R28,(1<<WGM01)
	OUT		TCCR0A,R28

	IN		R28,TCCR0B
	ORI		R28,0x01			//Prescaller clk/1
	OUT		TCCR0B,R28

	LDI		R28,TIMER0A_VAL		//
	OUT		OCR0A,R28

InitDDR:
	OUT		DDRB,R1				//PORTB for input
	LDI		R28,0b00001111
	OUT		DDRC,R28			//PORTC for output

	SEI

Start:
//test data
	LDI		R28,10
	STS		pwm_out+0,R28
	LDI		R28,80
	STS		pwm_out+1,R28
	LDI		R28,250
	STS		pwm_out+2,R28
	LDI		R28,220
	STS		pwm_out+3,R28
	LDI		R28,100
	STS		pwm_out+4,R28
	LDI		R28,200
	STS		pwm_out+5,R28
	LDI		R28,0
	STS		pwm_out+6,R28
	LDI		R28,120
	STS		pwm_out+7,R28


	LDI		R30,Low(2*TextMsg)
	LDI		R31,High(2*TextMsg)
	RCALL	PutStrFlash

LoadPWMToRegs:
	LDS		R2,pwm_out+0
	LDS		R3,pwm_out+1
	LDS		R4,pwm_out+2
	LDS		R5,pwm_out+3
	LDS		R6,pwm_out+4
	LDS		R7,pwm_out+5
	LDS		R8,pwm_out+6
	LDS		R9,pwm_out+7
	

MainLoop:
//	LDI		R28,0b00000011		//
//	OUT		PORTC,R28
//	LDI		R28,0b00001100
//	OUT		PORTC,R28
	
	RJMP	MainLoop	

;Subroutines
PutStrFlash:					//*Z - asciiz string in flash
	PUSH	R28
	PUSH	R30
	PUSH	R31
PutStrFlash_loop:
	LPM		R28,Z+
	CPI		R28,0
	BREQ	PutStrFlash_end
	RCALL	PutChar
	RJMP	PutStrFlash_loop
PutStrFlash_end:
	POP		R31
	POP		R30
	POP		R28
	RET

PutStr:							//*Z - asciiz string
	PUSH	R28
	PUSH	R30
	PUSH	R31
PutStr_loop:
	LD		R28,Z+
	CPI		R28,0
	BREQ	PutStr_end
	RCALL	PutChar
	RJMP	PutStr_loop
PutStr_end:
	POP		R31
	POP		R30
	POP		R28
	RET


PutChar:						//R28 - char
	PUSH	R28
	PUSH	R29
	PUSH	R30
	PUSH	R31
	CLI
	
	LDS		R30,uart_tx_wp
	CPI		R30,uart_tx_buf_size
	BRCC	TX_buf_overflow
	LDI		R31,High(uart_tx_buf)
	ADIW	R30,Low(uart_tx_buf)
	ST		Z+,R28
	SUBI	R30,Low(uart_tx_buf)
	STS		uart_tx_wp,R30

	LDS		R28,UCSR0A
	SBRS	R28,UDRE0
	RJMP	TX_in_progress
	MOV		R28,R30

	LDS		R30,uart_tx_rp
	LDI		R31,High(uart_tx_buf)
	ADIW	R30,Low(uart_tx_buf)
	LD		R29,Z+
	SUBI	R30,Low(uart_tx_buf)
	STS		UDR0,R29
	CP		R30,R28
	BRNE	TX_buf_not_empty_yet_putchar
TX_buf_is_empty_putchar:
	STS		uart_tx_wp,R1
	STS		uart_tx_rp,R1
TX_buf_not_empty_yet_putchar:
TX_in_progress:
TX_buf_overflow:
	SEI
	POP		R31
	POP		R30
	POP		R29
	POP		R28
	RET

;Interrupt vectors
TIMER0_COMPA_vect:
	PUSH	R28
	IN		R28,SREG
	PUSH	R28
	IN		R19,PORTB
	OUT		PORTC,R18

	SBIW	R26,1
	BREQ	TIMER0_out

	LDI		R28,1
	SUB		R2,R28
	ROR		R20		
	SUB		R3,R28
	ROR		R20		
	SUB		R4,R28
	ROR		R20		
	SUB		R5,R28
	ROR		R20		
	SUB		R6,R28
	ROR		R20		
	SUB		R7,R28
	ROR		R20		
	SUB		R8,R28
	ROR		R20		
	SUB		R9,R28
	ROR		R20
	SBRC	R27,0
	EOR		R18,R20		
	RJMP	TIMER0_ret
TIMER0_out:
	LDI		R26,Low(PWM_CYCLE)	// X register as timer
	LDI		R27,High(PWM_CYCLE)	// 200h for one PWM cycle
	LDS		R2,pwm_out+0
	LDS		R3,pwm_out+1
	LDS		R4,pwm_out+2
	LDS		R5,pwm_out+3
	LDS		R6,pwm_out+4
	LDS		R7,pwm_out+5
	LDS		R8,pwm_out+6
	LDS		R9,pwm_out+7
	LDI		R18,0xFF
TIMER0_ret:
	POP		R28
	OUT		SREG,R28
	POP		R28
	RETI

USART_RX_vect:
	PUSH	R28
	IN		R28,SREG
	PUSH	R28
	PUSH	R30
	PUSH	R31

	LDS		R28,UDR0
	LDS		R30,uart_rx_wp		//Load direct from data space
	CPI		R30,uart_rx_buf_size
	BRCC	RX_buf_overflow
	LDI		R31,High(uart_rx_buf)
	ADIW	R30,Low(uart_rx_buf)
	ST		Z+,R28
	SUBI	R30,Low(uart_rx_buf)
	STS		uart_rx_wp,R30

RX_buf_overflow:
	POP		R31
	POP		R30
	POP		R28
	OUT		SREG,R28
	POP		R28
	RETI

USART_TX_vect:
	PUSH	R28
	IN		R28,SREG
	PUSH	R28
	PUSH	R29
	PUSH	R30
	PUSH	R31

	LDS		R28,uart_tx_wp
	LDS		R30,uart_tx_rp
	CP		R30,R28
	BRCC	TX_buf_is_empty
	LDI		R31,High(uart_tx_buf)
	ADIW	R30,Low(uart_tx_buf)
	LD		R29,Z+
	SUBI	R30,Low(uart_tx_buf)
	STS		UDR0,R29
	CP		R30,R28
	BRNE	TX_buf_not_empty_yet
TX_buf_is_empty:
	STS		uart_tx_wp,R1
	STS		uart_tx_rp,R1
TX_buf_not_empty_yet:
	STS		uart_tx_rp,R30
	POP		R31
	POP		R30
	POP		R29
	POP		R28
	OUT		SREG,R28
	POP		R28
	RETI

TextMsg:
	.db		"V0.1",13,10,"READY",13,10,0


.ESEG
