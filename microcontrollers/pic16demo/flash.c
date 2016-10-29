/*
 * Define processor and include header file.
 */
#define __16f690
#include <pic14/pic16f690.h>

/*
 * Setup chip configuration
 */
unsigned __at 0x2007 __CONFIG =
	_FCMEN_OFF &		/* Fail-Safe Clock Monitor disabled */
	_IESO_OFF &		/* Internal External Switchover disabled */
	_BOR_OFF &		/* Brown-out Reset disabled */
	_CPD_OFF &		/* Data memory protection disabled */
	_CP_OFF &		/* Program memory protection disabled */
	_MCLRE_ON &		/* MCLR pin function is reset */
	_PWRTE_ON &		/* Power-up Timer enabled */
	_WDT_OFF &		/* Watchdog Timer disabled */
	_INTRC_OSC_NOCLKOUT;	/* Internal RC oscillator,
				 * I/O function on RA4 pin */

void mdelay (unsigned msec)
{
	unsigned char i;

	while (msec-- > 0) {
		for (i=0; i<200; i++)
			;
	}
}

void main()
{
	unsigned char count = 0;

	PORTC = count;
	TRISC = 0;
	while (1) {
		mdelay (250);
                PORTC = count++;
	}
}
