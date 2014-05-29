.include "m328def.inc"

	.equ	F_CPU = 16000000

.DSEG

rxbyte:
	.byte	1


.CSEG

.ORG 0x0000
	RJMP	Reset

.ORG 0x001A
Reset:
	LDI		R28,Low(RAMEND)     // Stack init
	LDI		R29,High(RAMEND)	// Y register as accumulator 
	OUT		SPH,R29
	OUT		SPL,R28
	CLR		R1            		// Clear register, R1 always zero

Init:
	LDI		R16,203
	STS		rxbyte,R16
	LDI		R16,(1<<0)			// B0 / Arduino #8 - output
	OUT		DDRB,R16
	LDI		R18,(0<<0)			// R18 - 0 for out B0
	LDI		R19,(1<<0)			// R19 - 1 for out B0

MainLoop:
	LDS		R16,rxbyte
	LDI		R17,8
Loop:
	OUT		PORTB,R19			// 1 - High level
	ROR		R16					// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	BRCS	Bit1				// 1/2
	OUT		PORTB,R18			// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	RJMP	Bit0				// 2
Bit1:
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
Bit0:	
	OUT		PORTB,R18			// 1	
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	NOP							// 1
	SUBI	R17,1				// 1
	BRNE	Loop				// 1/2

	RJMP	MainLoop	

