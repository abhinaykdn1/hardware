#include <avr/pgmspace.h>

unsigned char sinb[] PROGMEM = {128,130,133,136,139,143,146,149,
								152,155,158,161,164,167,170,173,
								176,178,181,184,187,190,192,195,
								198,200,203,205,208,210,212,215,
								217,219,221,223,225,227,229,231,
								233,234,236,238,239,240,242,243,
								244,245,247,248,249,249,250,251,
								252,252,253,253,253,254,254,255};

unsigned char sin(unsigned char x) //0 = 0, 2pi = 256, pi = 128; result -1 = 1, 0 = 128, 1 = 255
{
	//if 0..pi/2
	unsigned char xx = x;
	if (xx >= 128)
		xx = xx - 128;
	if (xx >= 64)
		xx = 64 - xx;
	unsigned char result = pgm_read_byte(&(sinb[xx]));
	if (x >= 128)
		result = 255-result;
	return result;
}
