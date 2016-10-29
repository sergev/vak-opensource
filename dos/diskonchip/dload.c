#include <stdio.h>
#include <dos.h>

#define BIOS		0xd800		/* target segment */
#define TARGET		0x8000		/* target segment */
#define NBYTES		0x2000		/* code size */

#define ChipID		(*(volatile unsigned short far*) MK_FP(BIOS,0x1000))
#define DOCControl	(*(volatile unsigned char far*) MK_FP(BIOS,0x1002))
#define CDSNControl	(*(volatile unsigned char far*) MK_FP(BIOS,0x1004))
#define CDSNSlowIO	(*(volatile unsigned char far*) MK_FP(BIOS,0x100d))
#define CDSN_IO 	(*(volatile unsigned char far*) MK_FP(BIOS,0x1800))

unsigned char wait_ready ()
{
	int count;

	ChipID++;
	for (count=1000; count>0; --count)
		if (CDSNControl & 0x80)	/* FR_B */
			return CDSNControl;
	printf ("Wait error, count = %d.\n", count);
	return CDSNControl;
}

void send_command (unsigned char cmd)
{
	CDSNControl = 0x1b;	/* FLASH_IO,WP,CLE,CE */
	ChipID++;
	CDSNSlowIO = cmd;
	CDSN_IO = cmd;

	wait_ready ();
}

void dump (char *filename, char far *data, int len)
{
	FILE *fd;

	fd = fopen (filename, "wb");
	if (! fd) {
		fprintf (stderr, "%s: cannot create\n", filename);
		return;
	}
	printf ("Dump %d bytes to %s.\n", len, filename);
	while (len-- > 0)
		fputc (*data++, fd);

	fclose (fd);
}

int main ()
{
	unsigned short dst, len;
	unsigned char sum, c;

	printf ("DiskOnChip loader.\n");

	DOCControl = 0x85;
	DOCControl = 0x85;
	send_command (0xff);
	send_command (0);

	printf ("Loading %d bytes to 0x%04x:0000.\n", NBYTES, TARGET);
	printf ("Disk initialized.\n");
	dst = 0;
	sum = 0;
	len = NBYTES;
	for (dst=0; dst<NBYTES; ++dst, --len) {
		if ((len & 0x0f) == 0) {
			printf (".");
			fflush (stdout);

			CDSNControl = 0x1d;	/* FLASH_IO,WP,ALE,CE */
			ChipID++;
			CDSNSlowIO = (unsigned char) dst;
			CDSN_IO = (unsigned char) dst;
			CDSNSlowIO = (unsigned char) (dst >> 8);
			CDSN_IO = (unsigned char) (dst >> 8);
			CDSNSlowIO = 0;
			CDSN_IO = 0;
			CDSNControl = 0x19;	/* FLASH_IO,WP,CE */

			wait_ready ();
		}
		c = CDSNSlowIO;
		c = CDSN_IO;
		sum += c;
		*(char far*) MK_FP (TARGET, dst) = c;
	}
	printf (" done\n", dst);
	if (sum != 0x55) {
		printf ("Checksum error: 0x%x (must be 0x55).\n", sum);
		exit (1);
	}
/*	dump ("boot2.bin", MK_FP (TARGET, 0), NBYTES); */

	printf ("Calling secondary boot at 0x%04x:0000.\n", TARGET);

	asm {
		push	CS
		mov	CX, offset back_label
		push	CX
		mov	CX, BIOS
		push	CX
		mov	CX, 83Eh
		push	CX
		mov	CX, TARGET
		push	CX
		mov	CX, 0
		push	CX
		db	0CBh	/* retf */
back_label:
	}

	printf ("Done.\n");
	return 0;
}
