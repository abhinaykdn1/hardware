

#define EXCLOCK_BUFF_ON (EXCLOCKBUFF_PORT |= (1<<EXCLOCKBUFF_PIN))
#define EXCLOCK_BUFF_OFF (EXCLOCKBUFF_PORT &= ~(1<<EXCLOCKBUFF_PIN))


void set_osc_pscale(unsigned char);
void set_osc_cmatch(unsigned char);
unsigned char get_osc_pscale(void);
unsigned char get_osc_cmatch(void);
void ExtClock_Init(void);
void SetOscOut(void);

void OscOutReInit0(void);
void OscOutReInit2(void);
//void OscOutReInit8(void);
