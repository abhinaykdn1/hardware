.include "m48def.inc"

	.equ	F_CPU = 14745600
	.equ	BAUDRATE = 57600
	.equ	BAUDDIVIDER = F_CPU/(16*BAUDRATE)-1
	.equ	MIN_TIME_US = 10 //10us = 0.01 ms
	.equ	TIMER0A_VAL = F_CPU/10000/MIN_TIME_US //147
	.equ	PWM_CYCLE = 512

;	PWMO0	R2
;	PWMO1	R3
;	PWMO2	R4
;	PWMO3	R5
;	PWMO4	R6
;	PWMO5	R7
;	PWMO6	R8
;	PWMO7	R9

;	LASTPORTC R18
;	LASTPORTB R19

;	TIMER LOW	R26
;	TIMER HIGH	R27


.DSEG

	.equ	uart_rx_buf_size = 32
	.equ	uart_tx_buf_size = 32
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
pwm_in_tmp:
	.byte	8

pwm_in_out_msk:
	.byte	8
pwm_in_out_mul: //PWMout = add + (PWMin * mul / 8)
	.byte	8
pwm_in_out_add:
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

InitPinChangeInt:
	LDS		R28,PCICR
	ORI		R28,(1<<PCIE0)		//Enable pinchange interrupts for PORTB
	STS		PCICR,R28
	LDS		R28,PCMSK0
	ORI		R28,0b00111111		//PORTB0 - PORTB5 pins for interrupt
	STS		PCMSK0,R28
	OUT		PORTB,R28			//PORTB0 - PORTB5 pull-up enable
	IN		R19,PINB			//Init R19 with current pin state

InitDDR:
	OUT		DDRB,R1				//PORTB for input
	LDI		R28,0b00111111
	OUT		DDRC,R28			//PORTC0 - PORTC5 for output

	SEI

Start:
//Default data
	LDI		R28,150
	STS		pwm_out+0,R28
	LDI		R28,150
	STS		pwm_out+1,R28
	LDI		R28,150
	STS		pwm_out+2,R28
	LDI		R28,150
	STS		pwm_out+3,R28
	LDI		R28,0
	STS		pwm_out+4,R28
	LDI		R28,0
	STS		pwm_out+5,R28
	LDI		R28,0
	STS		pwm_out+6,R28
	LDI		R28,0
	STS		pwm_out+7,R28

	LDI		R28,0
	STS		pwm_in_out_msk+0,R28
	LDI		R28,0
	STS		pwm_in_out_msk+1,R28
	LDI		R28,0
	STS		pwm_in_out_msk+2,R28
	LDI		R28,0
	STS		pwm_in_out_msk+3,R28
	LDI		R28,0
	STS		pwm_in_out_msk+4,R28
	LDI		R28,0
	STS		pwm_in_out_msk+5,R28
	LDI		R28,0
	STS		pwm_in_out_msk+6,R28
	LDI		R28,0
	STS		pwm_in_out_msk+7,R28

	LDI		R28,8
	STS		pwm_in_out_mul+0,R28
	LDI		R28,8
	STS		pwm_in_out_mul+1,R28
	LDI		R28,8
	STS		pwm_in_out_mul+2,R28
	LDI		R28,8
	STS		pwm_in_out_mul+3,R28
	LDI		R28,8
	STS		pwm_in_out_mul+4,R28
	LDI		R28,8
	STS		pwm_in_out_mul+5,R28
	LDI		R28,8
	STS		pwm_in_out_mul+6,R28
	LDI		R28,8
	STS		pwm_in_out_mul+7,R28

	LDI		R28,0
	STS		pwm_in_out_add+0,R28
	LDI		R28,0
	STS		pwm_in_out_add+1,R28
	LDI		R28,0
	STS		pwm_in_out_add+2,R28
	LDI		R28,0
	STS		pwm_in_out_add+3,R28
	LDI		R28,0
	STS		pwm_in_out_add+4,R28
	LDI		R28,0
	STS		pwm_in_out_add+5,R28
	LDI		R28,0
	STS		pwm_in_out_add+6,R28
	LDI		R28,0
	STS		pwm_in_out_add+7,R28

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
	RCALL	GetChar
	RCALL	PutChar
	RCALL	ProcessCommand
//	RCALL	PutHexByte
	
	RJMP	MainLoop	

ProcessCommand:					//In R28 - first letter of command
	PUSH	R28
	PUSH	R29
	PUSH	R30
	PUSH	R31
	ANDI	R28,~0x20
	CPI		R28,'H'
	BREQ	CommandHelp
	CPI		R28,'I'
	BREQ	CommandInputArray
	CPI		R28,'O'
	BREQ	CommandOutputArray
	CPI		R28,'T'
	BREQ	CommandTrimOpts
	CPI		R28,'S'
	BREQ	CommandSetOutputPWM
	CPI		R28,'M'
	BREQ	CommandSetPWMMask
	CPI		R28,'A'
	BREQ	CommandSetPWMAdd
	CPI		R28,'U'
	BREQ	CommandSetPWMMul
	CPI		R28,'Q'
	BREQ	CommandBreak

	RJMP	ProcessCommandEnd
CommandBreak:
	RJMP	ProcessCommandEnd
CommandHelp:
	LDI		R30,Low(2*HelpMsg)
	LDI		R31,High(2*HelpMsg)
	RCALL	PutStrFlash
	RJMP	ProcessCommandEnd
CommandInputArray:
	RCALL	PutPWMInArray
	RJMP	ProcessCommandEnd
CommandOutputArray:
	RCALL	PutPWMOutArray
	RJMP	ProcessCommandEnd
CommandTrimOpts:
	RCALL	PutPWMTrimOpts
	RJMP	ProcessCommandEnd
CommandSetOutputPWM:
	RCALL	GetChar
	ANDI	R28,0x07
	MOV		R30,R28
	LDI		R31,High(pwm_out)
	ADIW	R30,0x30
	ADIW	R30,Low(pwm_out)-0x30
	SUBI	R28,-'0'
	RCALL	PutChar
	RCALL	GetChar
	MOV		R29,R28
	RCALL	GetChar
	RCALL	HexToByte
	RCALL	PutHexByte
	STD		Z+0,R28
	RJMP	ProcessCommandEnd
CommandSetPWMMask:
	RCALL	GetChar
	ANDI	R28,0x07
	MOV		R30,R28
	LDI		R31,High(pwm_in_out_msk)
	ADIW	R30,0x30
	ADIW	R30,Low(pwm_in_out_msk)-0x30
	SUBI	R28,-'0'
	RCALL	PutChar
	RCALL	GetChar
	MOV		R29,R28
	RCALL	GetChar
	RCALL	HexToByte
	RCALL	PutHexByte
	STD		Z+0,R28
	RJMP	ProcessCommandEnd
CommandSetPWMAdd:
	RCALL	GetChar
	ANDI	R28,0x07
	MOV		R30,R28
	LDI		R31,High(pwm_in_out_add)
	ADIW	R30,0x30
	ADIW	R30,Low(pwm_in_out_add)-0x30
	SUBI	R28,-'0'
	RCALL	PutChar
	RCALL	GetChar
	MOV		R29,R28
	RCALL	GetChar
	RCALL	HexToByte
	RCALL	PutHexByte
	STD		Z+0,R28
	RJMP	ProcessCommandEnd
CommandSetPWMMul:
	RCALL	GetChar
	ANDI	R28,0x07
	MOV		R30,R28
	LDI		R31,High(pwm_in_out_mul)
	ADIW	R30,0x30
	ADIW	R30,Low(pwm_in_out_mul)-0x30
	SUBI	R28,-'0'
	RCALL	PutChar
	RCALL	GetChar
	MOV		R29,R28
	RCALL	GetChar
	RCALL	HexToByte
	RCALL	PutHexByte
	STD		Z+0,R28
	RJMP	ProcessCommandEnd
ProcessCommandEnd:
	POP		R31
	POP		R30
	POP		R29
	POP		R28
	RET

;Subroutines
HexToByte:		//R29 (Hi) R28 (Lo) hex -> R28 byte
	PUSH	R29
	SUBI	R29,'0'	
	CPI		R29,10
	BRCS	HexToByte_HiRdy
	SUBI	R29,-'0'
	ANDI	R29,~0x20
	SUBI	R29,'A'-10
HexToByte_HiRdy:
	SUBI	R28,'0'	
	CPI		R28,10
	BRCS	HexToByte_LoRdy
	SUBI	R28,-'0'
	ANDI	R28,~0x20
	SUBI	R28,'A'-10
HexToByte_LoRdy:
	ANDI	R29,0x0F
	ANDI	R28,0x0F
	LSL		R29
	LSL		R29
	LSL		R29
	LSL		R29
	OR		R28,R29
	POP		R29
	RET

CharReady:		// Return 1 in R28 if there is char in buffer
	PUSH	R29
	PUSH	R30
	LDI		R28,1
	LDS		R29,uart_rx_wp
	LDS		R30,uart_rx_rp
	CP		R30,R29 //if R30<R29, C=1 else R30>=R29 C=0
	BRCS	CharIsReady
	LDI		R28,0
CharIsReady:
	POP		R30
	POP		R29
	RET

GetChar:		// Return char in R28
	PUSH	R29
	PUSH	R30
	PUSH	R31
	SEI
GetChar_Wait:
	RCALL	CharReady
	TST		R28
	BREQ	GetChar_Wait
	CLI
	LDS		R28,uart_rx_wp
	LDS		R30,uart_rx_rp
	CP		R30,R28
	BRCC	RX_buf_is_empty
	LDI		R31,High(uart_rx_buf)
	ADIW	R30,Low(uart_rx_buf)
	LD		R29,Z+
	SUBI	R30,Low(uart_rx_buf)
	CP		R30,R28
	BRNE	RX_buf_not_empty_yet
RX_buf_is_empty:
	STS		uart_rx_wp,R1
	CLR		R30
RX_buf_not_empty_yet:
	STS		uart_rx_rp,R30
	MOV		R28,R29
	SEI
	POP		R31
	POP		R30
	POP		R29
	RET

PutPWMTrimOpts:
	PUSH	R28
	LDI		R28,'M'
	RCALL	PutChar
	LDS		R28,pwm_in_out_msk+0
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_msk+1
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_msk+2
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_msk+3
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_msk+4
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_msk+5
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_msk+6
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_msk+7
	RCALL	PutHexByte
	LDI		R28,'A'
	RCALL	PutChar
	LDS		R28,pwm_in_out_add+0
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_add+1
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_add+2
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_add+3
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_add+4
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_add+5
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_add+6
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_add+7
	RCALL	PutHexByte
	LDI		R28,'M'
	RCALL	PutChar
	LDS		R28,pwm_in_out_mul+0
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_mul+1
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_mul+2
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_mul+3
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_mul+4
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_mul+5
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_mul+6
	RCALL	PutHexByte
	LDS		R28,pwm_in_out_mul+7
	RCALL	PutHexByte
	POP		R28
	RET

PutPWMOutArray:
	PUSH	R28
	LDS		R28,pwm_out+0
	RCALL	PutHexByte
	LDS		R28,pwm_out+1
	RCALL	PutHexByte
	LDS		R28,pwm_out+2
	RCALL	PutHexByte
	LDS		R28,pwm_out+3
	RCALL	PutHexByte
	LDS		R28,pwm_out+4
	RCALL	PutHexByte
	LDS		R28,pwm_out+5
	RCALL	PutHexByte
	LDS		R28,pwm_out+6
	RCALL	PutHexByte
	LDS		R28,pwm_out+7
	RCALL	PutHexByte
	POP		R28
	RET

PutPWMInArray:
	PUSH	R28
	LDS		R28,pwm_in+0
	RCALL	PutHexByte
	LDS		R28,pwm_in+1
	RCALL	PutHexByte
	LDS		R28,pwm_in+2
	RCALL	PutHexByte
	LDS		R28,pwm_in+3
	RCALL	PutHexByte
	LDS		R28,pwm_in+4
	RCALL	PutHexByte
	LDS		R28,pwm_in+5
	RCALL	PutHexByte
	LDS		R28,pwm_in+6
	RCALL	PutHexByte
	LDS		R28,pwm_in+7
	RCALL	PutHexByte
	POP		R28
	RET

PutPWMOutRegArray:
	PUSH	R28
	MOV		R28,R2
	RCALL	PutHexByte
	MOV		R28,R3
	RCALL	PutHexByte
	MOV		R28,R4
	RCALL	PutHexByte
	MOV		R28,R5
	RCALL	PutHexByte
	MOV		R28,R6
	RCALL	PutHexByte
	MOV		R28,R7
	RCALL	PutHexByte
	MOV		R28,R8
	RCALL	PutHexByte
	MOV		R28,R9
	RCALL	PutHexByte
	POP		R28
	RET

PutPWMInRegArray:
	PUSH	R28
	MOV		R28,R10
	RCALL	PutHexByte
	MOV		R28,R11
	RCALL	PutHexByte
	MOV		R28,R12
	RCALL	PutHexByte
	MOV		R28,R13
	RCALL	PutHexByte
	MOV		R28,R14
	RCALL	PutHexByte
	MOV		R28,R15
	RCALL	PutHexByte
	MOV		R28,R16
	RCALL	PutHexByte
	MOV		R28,R17
	RCALL	PutHexByte
	POP		R28
	RET

PutHexByte:						//R28 - byte
	PUSH	R28
	PUSH	R28
	ANDI	R28,0xF0
	LSR		R28
	LSR		R28
	LSR		R28
	LSR		R28
	CPI		R28,10
	BRCS	PutHexByte_let0
	SUBI	R28,-('A'-'0'-10)
PutHexByte_let0:
	SUBI	R28,-'0'
	RCALL	PutChar
	POP		R28
	ANDI	R28,0x0F
	CPI		R28,10
	BRCS	PutHexByte_let1
	SUBI	R28,-('A'-'0'-10)
PutHexByte_let1:
	SUBI	R28,-'0'
	RCALL	PutChar
	POP		R28
	RET

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
TX_buf_ok:
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
	CLR		R30
TX_buf_not_empty_yet_putchar:
	STS		uart_tx_rp,R30
TX_in_progress:
	SEI
	POP		R31
	POP		R30
	POP		R29
	POP		R28
	RET
TX_buf_overflow:	//wait until buffer became empty
	SEI
	LDS		R30,uart_tx_wp
	CPI		R30,uart_tx_buf_size
	BRCC	TX_buf_overflow
	CLI
	RJMP	TX_buf_ok

;Interrupt vectors
PCI0_vect:
	PUSH	R24
	PUSH	R25
	PUSH	R28
	IN		R28,SREG
	PUSH	R28
	PUSH	R29
	PUSH	R30
	PUSH	R31

	MOV		R28,R19
	IN		R19,PINB
	PUSH	R19
	EOR		R19,R28
	
	LDI		R29,7
PCI0_vect_loop:
	ROL		R28
	ROL		R19			//In 0 bit old pin state, in carry - was it change
	BRCC	PCI0_vect_nextbit	
	SBRS	R19,0
	RJMP	PCI0_vect_bitWasSet
PCI0_vect_bitWasCleared: //Measure time, save result
	MOV		R30,R29
	LDI		R31,High(pwm_in_tmp) //In pwm_in_tmp - time of pulse raising edge
	ADIW	R30,0x30
	ADIW	R30,Low(pwm_in_tmp)-0x30
	LDD		R25,Z+0
	SUB		R25,R26
	MOV		R30,R29
	LDI		R31,High(pwm_in)
	ADIW	R30,0x30
	ADIW	R30,Low(pwm_in)-0x30
	STD		Z+0,R25

	MOV		R30,R29
	LDI		R31,High(pwm_in_out_msk)
	ADIW	R30,0x30
	ADIW	R30,Low(pwm_in_out_msk)-0x30
	LDD		R24,Z+0
	TST		R24
	BREQ	PCI0_vect_noTrim
PCI0_vect_trim:
	MOV		R30,R29
	LDI		R31,High(pwm_in_out_mul)
	ADIW	R30,0x30
	ADIW	R30,Low(pwm_in_out_mul)-0x30
	LDD		R24,Z+0

	PUSH	R0
	PUSH	R1

	MUL		R25,R24
	ROR		R1 			//DIV 8
	ROR		R0
	ROR		R1
	ROR		R0
	ROR		R1
	ROR		R0
	MOV		R25,R0

	POP		R1
	POP		R0

	MOV		R30,R29
	LDI		R31,High(pwm_in_out_add)
	ADIW	R30,0x30
	ADIW	R30,Low(pwm_in_out_add)-0x30
	LDD		R24,Z+0
	ADD		R25,R24

	MOV		R30,R29
	LDI		R31,High(pwm_out)
	ADIW	R30,0x30
	ADIW	R30,Low(pwm_out)-0x30
	STD		Z+0,R25
PCI0_vect_noTrim:
	RJMP	PCI0_vect_nextbit
PCI0_vect_bitWasSet:	 //Record time
	MOV		R30,R29
	LDI		R31,High(pwm_in_tmp) //In pwm_in_tmp - time of pulse raising edge
	ADIW	R30,0x30
	ADIW	R30,Low(pwm_in_tmp)-0x30
	MOV		R25,R26
	SUBI	R25,1
	STD		Z+0,R25
	RJMP	PCI0_vect_nextbit
PCI0_vect_nextbit:
	SUBI	R29,1
	BRCC	PCI0_vect_loop
	POP		R19

	POP		R31
	POP		R30
	POP		R29
	POP		R28
	OUT		SREG,R28
	POP		R28
	POP		R25
	POP		R24
	RETI

TIMER0_COMPA_vect:
	PUSH	R28
	IN		R28,SREG
	PUSH	R28
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

RX_buf_overflow:				//Ignore data from UART if buffer overflow
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
	CLR		R30
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
	.db		"PWM converter v0.1",13,10,0,0
HelpMsg:
	.db		13,10,"H - Help"
	.db		13,10,"I - Show input array"
	.db		13,10,"O - Show output array "
	.db		13,10,"T - Show trim info"
	.db		13,10,"Sxhh - Set x (0..8) output PWM to hh"
	.db		13,10,"Mxhh - Set PWM in-out mask"
	.db		13,10,"Axhh - Set PWM trim "
	.db		13,10,"Uxhh - Set PWM multiply "
	.db		13,10,0,0

.ESEG
