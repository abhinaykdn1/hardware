


 //ADC channell (PORTA)
#define ADCPIN 5


//Vcca pins in buffers. 0=Hi-Z
#define BUFERS_EN_PORT    PORTC
#define BUFERS_EN_DDR     DDRC
#define BUFERS_EN_PIN     PC2


//Vcca pin in ex. clock buffer.
#define EXCLOCKBUFF_PORT    PORTC
#define EXCLOCKBUFF_DDR     DDRC
#define EXCLOCKBUFF_PIN     PC1


//Reset Pin
#define RESET_PORT    PORTC
#define RESET_DDR     DDRC
#define RESET_PIN     PC0



//SUSPEND Pin (high level = suspend mode)
#define SUSPEND_PORT    PORTB
#define SUSPEND_DDR     DDRB
#define SUSPEND_PINX    PINB
#define SUSPEND_PIN     PB0


//BOOTLOADER activated Pin (low level - start bootloader)
#define BOOT_PORT    PORTD
#define BOOT_DDR     DDRD
#define BOOT_PINX    PIND
#define BOOT_PIN     PD4




/*		LED PINS SETTINGS		*/

//LED1 Pin
#define LED1_PORT    PORTD
#define LED1_DDR     DDRD
#define LED1_PIN     PD5

//LED2 Pin
#define LED2_PORT    PORTC
#define LED2_DDR     DDRC
#define LED2_PIN     PC4

//LED3 Pin
#define LED3_PORT    PORTB
#define LED3_DDR     DDRB
#define LED3_PIN     PB2

/*		LED PINS SETTINGS END		*/



