/*
 * Registers of reset controller.
 * Specific to QEMU "mips" machine.
 */
#define _REG32(a)	*(volatile unsigned*)(a)

#define HW_RESET	_REG32 (0x1fbf0000)
#define HW_SHUTDOWN	_REG32 (0x1fbf0004)

/*
 * Reset vector at 0xBFC00000
 */
asm (
       ".section .init \n"
"	.set	noreorder \n"
"	.org	0 \n"
"_reset_: \n"
"	la	$sp, _estack \n"
"	j	startup \n"
"	lui	$gp, 0 \n"
"	.section .text");

/*
 * Initialize memory and call main(), then shutdown.
 */
void startup ()
{
	/* Copy the .data image from flash to ram.
	 * Linker places it at the end of .text segment. */
	extern void _etext();
	extern unsigned __data_start, _edata, _end;
	unsigned *src = (unsigned*) &_etext;
	unsigned *dest = &__data_start;
	while (dest < &_edata)
		*dest++ = *src++;

	/* Clear .bss segment. */
	dest = &_edata;
	while (dest < &_end)
		*dest++ = 0;

	/* Call user program. */
	extern int main();
	main ();

	/* Stop qemu. */
	HW_SHUTDOWN = 42;
}
