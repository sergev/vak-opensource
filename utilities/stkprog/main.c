/*
 * Flash programmer for Atmel ATmega microcontrollers.
 *
 * By Sergey Vakulenko, <vak@cronyx.ru>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#ifndef WIN32
#	include <unistd.h>
#else
#	define strncasecmp strnicmp
#	define strcasecmp stricmp
#endif
#include <ctype.h>
#include "avr.h"

#define VERSION		"1.5"

/* Macros for converting between hex and binary. */
#define NIBBLE(x)	(isdigit(x) ? (x)-'0' : tolower(x)+10-'a')
#define HEX(buffer)	((NIBBLE((buffer)[0])<<4) + NIBBLE((buffer)[1]))

unsigned char write_data [0x40000];	/* To MCU internal flash write buffer - up to 256 kbytes */
u_int32_t srec_len;
u_int32_t srec_start = 0x1fffe;

avr_t *avr;
u_int32_t count;

int debug;
int program = 1;
int dont_lock;
int blocks_per_dot;

int do_options;
int option_m103c, option_wdton;
int option_ocden, option_jtagen, option_spien, option_ckopt;
int option_eesave, option_bootsz1, option_bootsz0, option_bootrst;
int option_bodlevel, option_boden, option_sut1, option_sut0;
int option_cksel3, option_cksel2, option_cksel1, option_cksel0;

#ifdef __unix__
#include "unix.c"
#endif

#if defined(__WIN32__) || defined(WIN32)
#include "mingw32.c"
#endif

/*
* Read the S record file.
*/
int read_srec (char *filename, unsigned char *output, u_int32_t *start)
{
	FILE *fd;
	unsigned char buf [256];
	unsigned char *data;
	u_int32_t address;
	int bytes, output_len;

	fd = fopen (filename, "r");
	if (! fd) {
		fprintf (stderr, "\n-- ");
		perror (filename);
		exit (1);
	}
	output_len = 0;
	while (fgets ((char*) buf, sizeof(buf), fd)) {
		if (buf[0] == '\n')
			continue;
		if (buf[0] != 'S') {
			fprintf (stderr, "%s: bad file format\n", filename);
			exit (1);
		}
		if (buf[1] == '7' || buf[1] == '8' || buf[1] == '9')
			break;

		/* Starting an S-record.  */
		if (! isxdigit (buf[2]) || ! isxdigit (buf[3])) {
			fprintf (stderr, "%s: bad record: %s\n", filename, buf);
			exit (1);
		}
		bytes = HEX (buf + 2);

		/* Ignore the checksum byte.  */
		--bytes;

		address = 0;
		data = buf + 4;
		switch (buf[1]) {
                case '3':
                        address = HEX (data);
                        data += 2;
                        --bytes;
                        /* Fall through.  */
                case '2':
                        address = (address << 8) | HEX (data);
                        data += 2;
                        --bytes;
                        /* Fall through.  */
                case '1':
                        address = (address << 8) | HEX (data);
                        data += 2;
                        address = (address << 8) | HEX (data);
                        data += 2;
                        bytes -= 2;

                        if (address+bytes >= 0x40000) {
                                fprintf (stderr, "%s: address too large: %s\n", filename, buf);
                                exit (1);
                        }
                        while (bytes-- > 0) {
                                if (address < *start)
                                        *start = address;
                                output[address++] = HEX (data);
                                data += 2;
                        }
                        if (output_len < address)
                                output_len = address;
                        break;
		}
	}
	return output_len;
}

void usage ()
{
	printf ("\nAtmel ATmega Programmer, Version " VERSION "\n");
	printf ("Copyright (C) 2000-2006 Serge Vakulenko, www.vak.ru\n");
	printf ("\nUsage:\n");
	printf ("\tmegaprog [-vnhuD] [-f dev] srec-file\n");
	printf ("Options:\n");
	printf ("\t-v\t\tverify only\n");
	printf ("\t-u\t\tleave memory unprotected\n");
	printf ("\t-D\t\tprint debugging information\n");
	printf ("\t-f dev\t\tserial device name, e.g. COM1 or /dev/cua01\n");
	printf ("\t-o option=0/1\toption for ATmega128; available options are:\n");
	printf ("\t\t\tM103C, WDTON, OCDEN, JTAGEN, SPIEN, CKOPT, EESAVE,\n");
	printf ("\t\t\tBOOTSZ1, BOOTSZ0, BOOTRST, BODLEVEL, BODEN,\n");
	printf ("\t\t\tSUT1, SUT0, CKSEL3, CKSEL2, CKSEL1, CKSEL0\n");
	printf ("\tsrec-file\tcode file in SREC format\n");
	exit (-1);
}

void print_symbols (char symbol, int cnt)
{
	while (cnt-- > 0)
		putchar (symbol);
}

void set_option (char *arg)
{
	static struct {
		char *option;
		int *ptr;
	} *p, tab [] = {
		{ "m103c=",	&option_m103c, },
		{ "wdton=",	&option_wdton, },
		{ "ocden=",	&option_ocden, },
		{ "jtagen=",	&option_jtagen, },
		{ "spien=",	&option_spien, },
		{ "ckopt=",	&option_ckopt, },
		{ "eesave=",	&option_eesave, },
		{ "bootsz1=",	&option_bootsz1, },
		{ "bootsz0=",	&option_bootsz0, },
		{ "bootrst=",	&option_bootrst, },
		{ "bodlevel=",	&option_bodlevel, },
		{ "boden=",	&option_boden, },
		{ "sut1=",	&option_sut1, },
		{ "sut0=",	&option_sut0, },
		{ "cksel3=",	&option_cksel3, },
		{ "cksel2=",	&option_cksel2, },
		{ "cksel1=",	&option_cksel1, },
		{ "cksel0=",	&option_cksel0, },
		{ 0,		0, },
	};
	int len;

	for (p=tab; p->option; ++p) {
		len = strlen (p->option);
		if (strncasecmp (arg, p->option, len) == 0) {
			if (arg[len] == '0')
				*p->ptr = -1;
			else if (arg[len] == '1')
				*p->ptr = 1;
			else
				break;
			do_options = 1;
			return;
		}
	}
	fprintf (stderr, "Incorrect option: `%s'\n", arg);
	usage ();
}

void quit (void)
{
	if (avr) {
		avr_close (avr);
		free (avr);
		avr = 0;
	}
}

int main (int argc, char **argv)
{
	char *devname = "/dev/cuaa0";
	char *sre_file = NULL;
	u_int32_t addr;
	int ch;
	extern char *optarg;
	extern int optind;
	void *t0;
	long left, used;

	while ((ch = getopt(argc, argv, "vDuf:o:")) != -1) {
		switch (ch) {
			case 'v':
				program = 0;
				break;
			case 'u':
				dont_lock = 1;
				break;
			case 'D':
				++debug;
				break;
			case 'f':
				devname = optarg;
				break;
			case 'o':
				set_option (optarg);
				break;
			default:
				fprintf (stderr, "Incorrect option: `%c'\n", ch);
				usage ();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 1) {
		fprintf (stderr, "-- Incorrect number of arguments: %d\n", argc);
		usage ();
	}
	if (strstr (argv[0], ".sre") == NULL) {
		printf ("-- Illegal filename (sre-file required): %s\n", argv[0]);
		usage ();
	} else
		sre_file = argv[0];

	srec_len = read_srec (sre_file, write_data, &srec_start);
	printf ("Memory: %#lx-%#lx\n", (unsigned long) srec_start, (unsigned long) srec_len);

	/* Open and detect the device. */
	atexit (quit);
	avr = avr_open (devname);

	if (program) {
		/* Erase flash and resyncronize. */
		avr_erase (avr);
	} else if (do_options) {
		fprintf (stderr, "Fuse options are allowed in program mode only\n");
		exit (1);
	}

	used = srec_len - srec_start;
	left = avr_flash_size (avr) - used;
	if (left < 0) {
		fprintf (stderr, "Device: %s, need %ld bytes of flash memory, but only %ld is available, %ld bytes out\n",
			avr_name (avr), (unsigned long) used, (unsigned long) avr_flash_size (avr), (unsigned long) -left);
		exit (1);
	} else {
		printf ("Device: %s, using %ld bytes of total %ld flash memory, %ld bytes free\n",
			avr_name (avr), (unsigned long) used, (unsigned long) avr_flash_size (avr), (unsigned long) left);
	}
#if 0
	if (avr_have_fuse (avr)) {
                unsigned char fuse_low = 0, fuse_high = 0, fuse_ext = 0;

		if(!program) {
			/* Read settings from device. */
			fuse_low = avr_read_fuse (avr);
			fuse_high = avr_read_fuse_high (avr);
			fuse_ext = avr_read_fuse_extended (avr);
		} else {
			/* Use default values. */
			fuse_low = 0xe1;
			fuse_high = 0x99;
			fuse_ext = 0xfd;
		}
		if (do_options) {
			/* Write fuse bits. */
			if (option_m103c || option_wdton) {
				if (option_m103c > 0)      fuse_ext |=  0x02;
				else if (option_m103c < 0) fuse_ext &= ~0x02;

				if (option_wdton > 0)      fuse_ext |=  0x01;
				else if (option_wdton < 0) fuse_ext &= ~0x01;

				avr_write_fuse_extended (avr, fuse_ext);
				fuse_ext = avr_read_fuse_extended (avr);
			}
			if (option_ocden || option_jtagen || option_spien ||
				option_ckopt || option_eesave || option_bootsz1 ||
				option_bootsz0 || option_bootrst) {
					if (option_ocden > 0)        fuse_high |=  0x80;
					else if (option_ocden < 0)   fuse_high &= ~0x80;

					if (option_jtagen > 0)       fuse_high |=  0x40;
					else if (option_jtagen < 0)  fuse_high &= ~0x40;

					if (option_spien > 0)        fuse_high |=  0x20;
					else if (option_spien < 0)   fuse_high &= ~0x20;

					if (option_ckopt > 0)        fuse_high |=  0x10;
					else if (option_ckopt < 0)   fuse_high &= ~0x10;

					if (option_eesave > 0)       fuse_high |=  0x08;
					else if (option_eesave < 0)  fuse_high &= ~0x08;

					if (option_bootsz1 > 0)      fuse_high |=  0x04;
					else if (option_bootsz1 < 0) fuse_high &= ~0x04;

					if (option_bootsz0 > 0)      fuse_high |=  0x02;
					else if (option_bootsz0 < 0) fuse_high &= ~0x02;

					if (option_bootrst > 0)      fuse_high |=  0x01;
					else if (option_bootrst < 0) fuse_high &= ~0x01;

					avr_write_fuse_high (avr, fuse_high);
					fuse_high = avr_read_fuse_high (avr);
				}
				if (option_bodlevel || option_boden || option_sut1 ||
					option_sut0 || option_cksel3 || option_cksel2 ||
					option_cksel1 || option_cksel0) {
						if (option_bodlevel > 0)      fuse_low |=  0x80;
						else if (option_bodlevel < 0) fuse_low &= ~0x80;

						if (option_boden > 0)         fuse_low |=  0x40;
						else if (option_boden < 0)    fuse_low &= ~0x40;

						if (option_sut1 > 0)          fuse_low |=  0x20;
						else if (option_sut1 < 0)     fuse_low &= ~0x20;

						if (option_sut0 > 0)          fuse_low |=  0x10;
						else if (option_sut0 < 0)     fuse_low &= ~0x10;

						if (option_cksel3 > 0)        fuse_low |=  0x08;
						else if (option_cksel3 < 0)   fuse_low &= ~0x08;

						if (option_cksel2 > 0)        fuse_low |=  0x04;
						else if (option_cksel2 < 0)   fuse_low &= ~0x04;

						if (option_cksel1 > 0)        fuse_low |=  0x02;
						else if (option_cksel1 < 0)   fuse_low &= ~0x02;

						if (option_cksel0 > 0)        fuse_low |=  0x01;
						else if (option_cksel0 < 0)   fuse_low &= ~0x01;

						avr_write_fuse (avr, fuse_low);
						fuse_low = avr_read_fuse (avr);
					}
		}
		printf ("Fuses:");
		if (! (fuse_ext  & 0x02)) printf (" M103C");
		if (! (fuse_ext  & 0x01)) printf (" WDTON");
		if (! (fuse_high & 0x80)) printf (" OCDEN");
		if (! (fuse_high & 0x40)) printf (" JTAGEN");
		if (! (fuse_high & 0x20)) printf (" SPIEN");
		if (! (fuse_high & 0x10)) printf (" CKOPT");
		if (! (fuse_high & 0x08)) printf (" EESAVE");
		if (! (fuse_high & 0x04)) printf (" BOOTSZ1");
		if (! (fuse_high & 0x02)) printf (" BOOTSZ0");
		if (! (fuse_high & 0x01)) printf (" BOOTRST");
		if (! (fuse_low  & 0x80)) printf (" BODLEVEL");
		if (! (fuse_low  & 0x40)) printf (" BODEN");
		if (! (fuse_low  & 0x20)) printf (" SUT1");
		if (! (fuse_low  & 0x10)) printf (" SUT0");
		if (! (fuse_low  & 0x08)) printf (" CKSEL3");
		if (! (fuse_low  & 0x04)) printf (" CKSEL2");
		if (! (fuse_low  & 0x02)) printf (" CKSEL1");
		if (! (fuse_low  & 0x01)) printf (" CKSEL0");
		printf ("\n");
	} else if (do_options) {
		printf ("WARNING: device has no fuses\n");
		do_options = 0;
	}
#endif
	blocks_per_dot = 8;
	for (;;) {
		count = 1 + (srec_len - srec_start) /
			256 / blocks_per_dot;
		if (count > 32 || blocks_per_dot == 1)
			break;
		blocks_per_dot /= 2;
	}
	printf (program ? "Program: " : "Verify: ");
	print_symbols ('.', count);
	print_symbols ('\b', count);
	fflush (stdout);

	count = 0;
	t0 = fix_time ();
	for (addr=srec_start; addr<srec_len; addr+=256) {
                if (program) {
                        avr_write_block (avr, addr, write_data+addr, 256);
                }
                ++count;
                fprintf (stderr, "%c\b%s", "/-\\|" [count & 3],
                        (count % blocks_per_dot == 0) ? "#" : "");
                fflush (stderr);

		if (! avr_check_block (avr, addr, write_data+addr, 256)) {
                        printf ("Failed.\n");
                        exit (1);
		}
	}

	printf ("#\nDone in %ld seconds, %ld bytes per second\n",
		(unsigned long) seconds_elapsed (t0),
                (unsigned long) (srec_len - srec_start) / seconds_elapsed (t0));
#if 0
	if (program && ! dont_lock) {
		/* Write lock bits. */
		avr_lock (avr);
		printf ("Memory read/write protection enabled.\n");
	}
#endif
	return 0;
}
