.ORG INT0addr	;External Interrupt0
	RETI
.ORG INT1addr	;External Interrupt1
	RETI
.ORG PCI0addr	;Pin Change Interrupt0
	RJMP	PCI0_vect
.ORG PCI1addr	;Pin Change Interrupt1
	RETI
.ORG PCI2addr	;Pin Change Interrupt2
	RETI
.ORG WDTaddr	;Watchdog Timeout
	RETI
.ORG OC2Aaddr	;Timer/Counter2 Compare Match Interrupt
	RETI
.ORG OC2Baddr	;Timer/Counter2 Compare Match Interrupt	
	RETI
.ORG OVF2addr	;Overflow2 Interrupt
	RETI
.ORG ICP1addr	;Input Capture1 Interrupt 	
	RETI
.ORG OC1Aaddr	;Output Compare1A Interrupt 
	RETI
.ORG OC1Baddr	;Output Compare1B Interrupt 
	RETI
.ORG OVF1addr	;Overflow1 Interrupt 
	RETI
.ORG OC0Aaddr	;Timer/Counter0 Compare Match Interrupt
	RJMP	TIMER0_COMPA_vect
.ORG OC0Baddr	;Timer/Counter0 Compare Match Interrupt	
	RETI
.ORG OVF0addr	;Overflow0 Interrupt
	RETI
.ORG SPIaddr	;SPI Interrupt 	
	RETI
.ORG URXCaddr	;USART Receive Complete Interrupt 
	RJMP	USART_RX_vect
.ORG UDREaddr	;USART Data Register Empty Interrupt 
	RETI
.ORG UTXCaddr	;USART Transmit Complete Interrupt 
	RJMP	USART_TX_vect
.ORG ADCCaddr	;ADC Conversion Complete Handle
    RETI
.ORG ERDYaddr	;EEPROM write complete
	RETI
.ORG ACIaddr	;Analog Comparator Interrupt 
	RETI
.ORG TWIaddr	;TWI Interrupt Vector Address
    RETI
.ORG SPMRaddr	;Store Program Memory Ready Interrupt 
	RETI
