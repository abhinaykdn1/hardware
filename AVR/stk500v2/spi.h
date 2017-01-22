/* vim: set sw=8 ts=8 si et: */
/*************************************************************************
 Title:   C include file for spi
 Target:    atmega8
 Copyright: GPL
***************************************************************************/
#ifndef SPI_H
#define SPI_H



//#define RSTINVERT //Define it for use NPN transistor instead IC3 buffer (RESET)





#define MISO_IN  {DDRB &= ~(1<<PB4); PORTB &= ~(1<<PB4);}
#define MISO_IN_P  {DDRB &= ~(1<<PB4); PORTB |= (1<PB4);}

#define MOSI_OUT {DDRB |= (1<<PB3); PORTB &= ~ (1<<PB3);}
#define MOSI_IN_P  {DDRB &= ~(1<<PB3); PORTB |= (1<PB3);}

#define SCK_OUT {DDRB |= (1<<PB5); PORTB &= ~(1<<PB5);}
#define SCK_IN_P  {DDRB &= ~(1<<PB5); PORTB |= (1<PB5);}

#define RESET_OUT {RESET_DDR |= (1<<RESET_PIN);}
#define RESET_IN_P  {RESET_DDR &= ~(1<<RESET_PIN); RESET_PORT |= (1<RESET_PIN);}

#define BUFFS_ON {BUFERS_EN_PORT |= (1<<BUFERS_EN_PIN);}
#define BUFFS_OFF {BUFERS_EN_PORT &= ~(1<<BUFERS_EN_PIN);} //Hi-Z


#define RESET_0 {RESET_PORT &= ~(1<<RESET_PIN);}
#define RESET_1 {RESET_PORT |= (1<<RESET_PIN);}


#ifndef RSTINVERT
void set_reset_polarity(char);
char get_reset_polarity(void);
#endif
extern void spi_init(void);
extern void spi_start(void);
extern void spi_set_sck_duration(unsigned char);
extern unsigned char spi_get_sck_duration(void);
extern void spi_mastertransmit_nr(unsigned char);
extern unsigned char spi_mastertransmit(unsigned char);
extern void software_spi_mastertransmit_nr(unsigned char);
extern unsigned char software_spi_mastertransmit(unsigned char);
extern void hardware_spi_mastertransmit_nr(unsigned char);
extern unsigned char hardware_spi_mastertransmit(unsigned char);
extern void spi_mastertransmit_16_nr(unsigned int);
extern unsigned char spi_mastertransmit_32(unsigned long);
extern void spi_stop(void);






void set_spi_speed(char);

#endif /* SPI_H */
