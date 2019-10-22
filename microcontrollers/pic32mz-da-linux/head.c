/*
 * PIC32MZ2064DAG169 or PIC32MZ2064DAA288 head.
 */

/*
 * devcfg3 DDR size.
 */
#ifdef PIC32_MEM_32M
#define	DDR_SIZE	0x00050000
#else
#define	DDR_SIZE	0x000F0000
#endif

/*
 * eFUSE
 */
const unsigned int __devcfg[5] __attribute__ ((section (".devcfg"))) = {
	0xF4FFFFFF,		/* devcfg4 */
	0xFEF0FFFF | DDR_SIZE,	/* devcfg3 */
	0xF7F9B11A,		/* devcfg2 */
	0x5F74FCF9,		/* devcfg1 */
	0xF7FFFFD3		/* devcfg0 */
};

/*
 * Jump to prog memory at 0x1D000000.
 */
const unsigned int __cpu_reset[2] __attribute__ ((section (".reset"))) = {
	0x0B401000,	/* j 0x9D004000 <main> */
	0x00000000	/* nop */
};

/*
 * dummy.
 */
void
dummy (void)
{
}
