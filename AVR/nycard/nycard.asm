.include "tn13def.inc"

.def	voice1n		= R2
.def	voice2n		= R3
.def	voice3n		= R4

.def	voice1nn	= R5
.def	voice2nn	= R6
.def	voice3nn	= R7

.def	voice1ncl	= R8
.def	voice1nch	= R9
.def	voice2ncl	= R10
.def	voice2nch	= R11
.def	voice3ncl	= R12
.def	voice3nch	= R13

.def	notecountl	= R14
.def	notecounth	= R15
.def	temp		= R16
.def	temph		= R17
.def	tempocounth	= R18
.def	tempocountl	= R19

.def	voice1l		= R20
.def	voice2l		= R21
.def	voice3l		= R22

.def	outpin		= R23

.equ 	XTAL = 4800000
.equ	ProgSteps = 173 //Program steps in one cycle
.equ	MaxFreq = XTAL / ProgSteps 	
.equ 	Tempo = 160 // 1/4 per minute
.equ	TempoCounter = (MaxFreq * 60 / 2) / Tempo 

.cseg

.org	$000
		rjmp	Init
Init:
		ldi 	temp, (1<<PB3) | (1<<PB4)
		out 	DDRB, temp
Start:
		clr		tempocountl
		clr		tempocounth
		clr		notecountl
		clr		notecounth
		clr		voice1n
		clr		voice2n
		clr		voice3n
		clr		voice1nn
		clr		voice2nn
		clr		voice3nn
		clr		voice1ncl
		clr		voice1nch
		clr		voice2ncl
		clr		voice2nch
		clr		voice3ncl
		clr		voice3nch
		clr		voice1l
		clr		voice2l
		clr		voice3l
MainLoop:
		clr		outpin
		subi	tempocountl,1
		sbci	tempocounth,0
		brcs	NewNote
		nop
		nop
		ldi		temp,33
Delay1:
		dec		temp
		brne	Delay1
		rjmp	EndNewNote
NewNote:
		ldi		tempocountl, low(TempoCounter)
		ldi		tempocounth, high(TempoCounter)
		ldi		temp, 1
		clr		temph
		add		notecountl, temp
		adc		notecounth, temph
		mov		temph, voice1nch
		mov		temp, voice1ncl
		sub		temp, notecountl
		sbc		temph, notecounth
		brcs	LoadNoteVoice1
		nop
		ldi		temp,8
Delay2:
		dec		temp
		brne	Delay2
		rjmp	EndLdVoice1
LoadNoteVoice1:
		ldi		temp, low(Voice1*2) //load voice table
		mov		ZL, temp
		ldi		temp, high(Voice1*2)
		mov		ZH, temp
		clr		temph	//calculate offset
		add		ZL, voice1nn
		adc		ZH, temph
		lpm
		adc		R0, temph
		brne	Cont1
		rjmp	Start
Cont1:
		mov		temp,R0
		andi	temp,0xE0
		lsr		temp
		lsr		temp
		lsr		temp
		lsr		temp
		lsr		temp
		add		voice1ncl, temp
		adc		voice1nch, temph //temph must be 0
		mov		temp,R0
		andi	temp,0x1F
		mov		voice1n,temp
		inc		voice1nn
EndLdVoice1:
		mov		temph, voice2nch
		mov		temp, voice2ncl
		sub		temp, notecountl
		sbc		temph, notecounth
		brcs	LoadNoteVoice2
		nop
		ldi		temp,8
Delay3:
		dec		temp
		brne	Delay3
		rjmp	EndLdVoice2
LoadNoteVoice2:
		ldi		temp, low(Voice2*2) //load voice table
		mov		ZL, temp
		ldi		temp, high(Voice2*2)
		mov		ZH, temp
		clr		temph	//calculate offset
		add		ZL, voice2nn
		adc		ZH, temph
		lpm
		adc		R0, temph
		brne	Cont2
		rjmp	Start
Cont2:
		mov		temp,R0
		andi	temp,0xE0
		lsr		temp
		lsr		temp
		lsr		temp
		lsr		temp
		lsr		temp
		add		voice2ncl, temp
		adc		voice2nch, temph //temph must be 0
		mov		temp,R0
		andi	temp,0x1F
		mov		voice2n,temp
		inc		voice2nn
EndLdVoice2:
		mov		temph, voice3nch
		mov		temp, voice3ncl
		sub		temp, notecountl
		sbc		temph, notecounth
		brcs	LoadNoteVoice3
		nop
		ldi		temp,8
Delay4:
		dec		temp
		brne	Delay4
		rjmp	EndLdVoice3
LoadNoteVoice3:
		ldi		temp, low(Voice3*2) //load voice table
		mov		ZL, temp
		ldi		temp, high(Voice3*2)
		mov		ZH, temp
		clr		temph	//calculate offset
		add		ZL, voice3nn
		adc		ZH, temph
		lpm
		adc		R0, temph
		brne	Cont3
		rjmp	Start
Cont3:
		mov		temp,R0
		andi	temp,0xE0
		lsr		temp
		lsr		temp
		lsr		temp
		lsr		temp
		lsr		temp
		add		voice3ncl, temp
		adc		voice3nch, temph //temph must be 0
		mov		temp,R0
		andi	temp,0x1F
		mov		voice3n,temp
		inc		voice3nn
EndLdVoice3:
EndNewNote:
		ldi		temp, low(NotesFreq*2) //load frequency table
		mov		ZL, temp
		ldi		temp, high(NotesFreq*2)
		mov		ZH, temp
		clr		temph	//calculate offset
		add		ZL, voice1n
		adc		ZH, temph		
		subi	voice1l,1
		brcs	Reload1
		lpm
		mov		temp,R0
		ldi		temp, 2
		sub		temp,voice1l
		brcs	Bit1Off
		inc		outpin
Bit1Off:
		rjmp	Endload1
Reload1:
		lpm
		mov		voice1l,R0
		nop
		nop
		nop
		nop
		nop
Endload1:
		ldi		temp, low(NotesFreq*2) //load frequency table
		mov		ZL, temp
		ldi		temp, high(NotesFreq*2)
		mov		ZH, temp
		clr		temph	//calculate offset
		add		ZL, voice2n
		adc		ZH, temph		
		subi	voice2l,1
		brcs	Reload2
		lpm
		mov		temp,R0
		ldi		temp, 2
		sub		temp,voice2l
		brcs	Bit2Off
		inc		outpin
Bit2Off:
		rjmp	Endload2
Reload2:
		lpm
		mov		voice2l,R0
		nop
		nop
		nop
		nop
		nop
Endload2:
		ldi		temp, low(NotesFreq*2) //load frequency table
		mov		ZL, temp
		ldi		temp, high(NotesFreq*2)
		mov		ZH, temp
		clr		temph	//calculate offset
		add		ZL, voice3n
		adc		ZH, temph		
		subi	voice3l,1
		brcs	Reload3
		lpm
		mov		temp,R0
		ldi		temp,2
		sub		temp,voice3l
		brcs	Bit3Off
		inc		outpin
Bit3Off:
		rjmp	Endload3
Reload3:
		lpm
		mov		voice3l,R0
		nop
		nop
		nop
		nop
		nop
Endload3:
		ori		outpin,0
		breq	SetPin1
		nop
SetPin0:
		sbi		PORTB, 3
		cbi		PORTB, 4
		rjmp	MainLoop
SetPin1:
		cbi		PORTB, 3
		sbi		PORTB, 4
		rjmp	MainLoop

Voice1:
		.db 205, 207, 209, 77, 79, 81, 210, 208
		.db 206, 205, 210, 208, 206, 205, 134, 72
		.db 137, 75, 141, 77, 198, 193, 133, 70
		.db 193, 133, 70, 210, 208, 206, 205, 210
		.db 208, 206, 205, 134, 72, 137, 75, 141
		.db 77, 198, 193, 133, 70, 193, 133, 70
		.db 193, 195, 197, 65, 67, 69, 198, 192
		.db 192, 0
Voice2:
		.db 213, 212, 213, 192, 192, 192, 192, 192
		.db 213, 213, 213, 213, 213, 210, 209, 210
		.db 192, 148, 85, 192, 148, 85, 192, 192
		.db 192, 192, 213, 213, 213, 213, 213, 210
		.db 209, 210, 192, 148, 85, 192, 148, 85
		.db 192, 192, 213, 213, 192, 192, 192, 0
Voice3:
		.db 217, 215, 217, 192, 85, 52, 53, 82
		.db 85, 52, 53, 82, 85, 52, 53, 82
		.db 85, 52, 53, 82, 89, 55, 57, 85
		.db 89, 55, 57, 85, 89, 55, 57, 85
		.db 89, 55, 57, 85, 94, 62, 62, 60
		.db 58, 89, 57, 57, 55, 53, 87, 55
		.db 55, 57, 55, 85, 52, 53, 82, 45
		.db 47, 49, 50, 52, 53, 55, 57, 87
		.db 85, 45, 47, 49, 50, 52, 53, 55
		.db 57, 87, 85, 85, 52, 53, 82, 85
		.db 52, 53, 82, 85, 52, 53, 82, 85
		.db 52, 53, 82, 89, 55, 57, 85, 89
		.db 55, 57, 85, 89, 55, 57, 85, 89
		.db 55, 57, 85, 94, 62, 62, 60, 58
		.db 89, 57, 57, 55, 53, 87, 55, 55
		.db 57, 55, 85, 52, 53, 82, 45, 47
		.db 49, 50, 52, 53, 55, 57, 87, 85
		.db 45, 47, 49, 50, 52, 53, 55, 57
		.db 87, 85, 85, 52, 53, 82, 85, 52
		.db 53, 82, 89, 55, 57, 85, 89, 55
		.db 57, 85, 210, 192, 192, 0

.equ 	fr_A2	= MaxFreq / 110
.equ	fr_Hb2	= MaxFreq / 116.540940379522
.equ	fr_H2	= MaxFreq / 123.470825314031
.equ	fr_C3	= MaxFreq / 130.812782650299
.equ	fr_Db3	= MaxFreq / 138.591315488436
.equ	fr_D3	= MaxFreq / 146.832383958704
.equ	fr_Eb3	= MaxFreq / 155.56349186104
.equ	fr_E3	= MaxFreq / 164.813778456435
.equ	fr_F3	= MaxFreq / 174.614115716502
.equ	fr_Gb3	= MaxFreq / 184.997211355817
.equ	fr_G3	= MaxFreq / 195.997717990875
.equ	fr_Ab3	= MaxFreq / 207.652348789973
.equ	fr_A3	= MaxFreq / 220
.equ	fr_Hb3	= MaxFreq / 233.081880759045
.equ	fr_H3	= MaxFreq / 246.941650628062
.equ	fr_C4	= MaxFreq / 261.625565300599
.equ	fr_Db4	= MaxFreq / 277.182630976872
.equ	fr_D4	= MaxFreq / 293.664767917408
.equ	fr_Eb4	= MaxFreq / 311.126983722081
.equ	fr_E4	= MaxFreq / 329.62755691287
.equ	fr_F4	= MaxFreq / 349.228231433004
.equ	fr_Gb4	= MaxFreq / 369.994422711635
.equ	fr_G4	= MaxFreq / 391.995435981749
.equ	fr_Ab4	= MaxFreq / 415.304697579945
.equ	fr_A4	= MaxFreq / 440
.equ	fr_Hb4	= MaxFreq / 466.16376151809
.equ	fr_H4	= MaxFreq / 493.883301256124
.equ	fr_C5	= MaxFreq / 523.251130601198
.equ	fr_Db5	= MaxFreq / 554.365261953745
.equ	fr_D5	= MaxFreq / 587.329535834816
.equ	fr_Eb5	= MaxFreq / 622.253967444162
.equ	fr_E5	= MaxFreq / 659.255113825741
.equ	fr_F5	= MaxFreq / 698.456462866008
.equ	fr_Gb5	= MaxFreq / 739.98884542327
.equ	fr_G5	= MaxFreq / 783.990871963499
.equ	fr_Ab5	= MaxFreq / 830.609395159891
.equ	fr_A5	= MaxFreq / 880

NotesFreq:
		.db 		0,	fr_D3,	fr_Eb3,	fr_E3,	fr_F3,	fr_Gb3,	fr_G3,	fr_Ab3
		.db		fr_A3,	fr_Hb3,	fr_H3, fr_C4,  fr_Db4,	fr_D4,	fr_Eb4, fr_E4
		.db		fr_F4,  fr_Gb4, fr_G4,  fr_Ab4, fr_A4,  fr_Hb4, fr_H4,	fr_C5
		.db		fr_Db5,	fr_D5,	fr_Eb5, fr_E5,  fr_F5,  fr_Gb5, fr_G5