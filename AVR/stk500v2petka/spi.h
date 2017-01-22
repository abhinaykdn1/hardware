/* vim: set sw=8 ts=8 si et: */
/*************************************************************************
 Title:   C include file for spi
 Target:    atmega8
 Copyright: GPL
***************************************************************************/
#ifndef SPI_H
#define SPI_H


#define spi_scklow() bit_off(TG_SCK)

// SCK signal of target uC
#define TG_SCK_PORT		D 
#define TG_SCK_PIN		4 
#define TG_SCK_ALEV		H 

// MISO signal of target uC
#define TG_MISO_PORT		D 
#define TG_MISO_PIN		3 
#define TG_MISO_ALEV		H 

// RESET signal of target uC
#define TG_RESET_PORT		B 
#define TG_RESET_PIN		0 
#define TG_RESET_ALEV		H 

// MOSI signal of target uC
#define TG_MOSI_PORT		D 
#define TG_MOSI_PIN		2 
#define TG_MOSI_ALEV		H 





extern void spi_init(void);
extern unsigned char spi_set_sck_duration(unsigned char dur);
extern unsigned char spi_get_sck_duration(void);
extern void spi_mastertransmit_nr(unsigned char data);
extern unsigned char spi_mastertransmit(unsigned char data);
extern void spi_mastertransmit_16_nr(unsigned int data);
extern unsigned char spi_mastertransmit_32(unsigned long data);
extern void spi_disable(void);
extern void spi_reset_pulse(void);
extern void spi_sck_pulse(void);

#endif /* SPI_H */
