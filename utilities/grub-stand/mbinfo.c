/*
 * An example of standalone program: display data from Multiboot
 * information structure.
 * Copyright (C) 1999 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "multiboot.h"

/*
 * Some screen stuff.
 */
#define COLUMNS			80	/* The number of columns. */
#define LINES			24	/* The number of lines. */
#define ATTRIBUTE		7	/* The attribute of an character. */
#define VIDEO			0xB8000	/* The video memory address. */

static int xpos;			/* Save the X position. */
static int ypos;			/* Save the Y position. */
static volatile unsigned char *video;	/* Point to the video memory. */

/*
 * Clear the screen and initialize VIDEO, XPOS and YPOS.
 */
static void clear_screen (void)
{
	int i;

	video = (unsigned char*) VIDEO;

	for (i=0; i<COLUMNS*LINES*2; i++)
		*(video + i) = 0;

	xpos = 0;
	ypos = 0;
}

/*
 * Convert the integer D to a string and save the string in BUF. If
 * BASE is equal to 'd', interpret that D is decimal, and if BASE is
 * equal to 'x', interpret that D is hexadecimal.
 */
static void itoa (char *buf, int base, int d)
{
	char *p = buf;
	char *p1, *p2;
	unsigned long ud = d;
	int divisor = 10;

	/* If %d is specified and D is minus, put `-' in the head. */
	if (base == 'd' && d < 0) {
		*p++ = '-';
		buf++;
		ud = -d;
	} else if (base == 'x')
		divisor = 16;

	/* Divide UD by DIVISOR until UD == 0. */
	do {
		int remainder = ud % divisor;

		*p++ = (remainder < 10) ? remainder + '0' :
			remainder + 'a' - 10;
	}
	while (ud /= divisor);

	/* Terminate BUF. */
	*p = 0;

	/* Reverse BUF. */
	p1 = buf;
	p2 = p - 1;
	while (p1 < p2) {
		char tmp = *p1;

		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

/*
 * Put the character C on the screen.
 */
static void putchar (int c)
{
	if (c == '\n' || c == '\r') {
newline:	xpos = 0;
		ypos++;
		if (ypos >= LINES)
			ypos = 0;
		return;
	}
	*(video + (xpos + ypos * COLUMNS) * 2) = c;
	*(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;

	xpos++;
	if (xpos >= COLUMNS)
		goto newline;
}

/*
 * Format a string and print it on the screen, just like the libc
 * function printf.
 */
void printf (const char *format, ...)
{
	char **arg = (char**) &format;
	int c;
	char buf[20], *p;

	arg++;

	while ((c = *format++) != 0) {
		if (c != '%') {
			putchar (c);
			continue;
		}
		c = *format++;
		switch (c) {
		case 'd':
		case 'u':
		case 'x':
			itoa (buf, c, *((int*) arg++));
			p = buf;
			goto string;
		case 's':
			p = *arg++;
			if (! p)
				p = "(null)";
string:			while (*p)
				putchar (*p++);
			break;
		default:
			putchar (*((int*) arg++));
			break;
		}
	}
}

/*
 * Check if MAGIC is valid and print the Multiboot information structure
 * pointed by ADDR.
 */
void cmain (unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;

	/* Clear the screen. */
	clear_screen ();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		printf ("Invalid magic number: 0x%x\n", (unsigned)magic);
		return;
	}
	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t*) addr;

	/* Is the boot loader name passed? */
	if (mbi->flags & MB_INFO_LOADER_NAME)
		printf ("loader = %s\n", (char*) mbi->boot_loader_name);

	/* Print out the flags. */
	printf ("flags = 0x%x\n", (unsigned)mbi->flags);

	/* Are mem_* valid? */
	if (mbi->flags & MB_INFO_MEMORY)
		printf ("mem_lower = %uKB, mem_upper = %uKB\n",
		    (unsigned)mbi->mem_lower, (unsigned)mbi->mem_upper);

	/* Is boot_device valid? */
	if (mbi->boot_device >> 24 == 0xff)
		mbi->flags &= ~MB_INFO_BOOTDEV;
	if (mbi->flags & MB_INFO_BOOTDEV)
		printf ("boot_device = 0x%x, partition = 0x%x\n",
			(unsigned)mbi->boot_device >> 24,
			(unsigned)mbi->boot_device & 0xffffff);

	/* Is the command line passed? */
	if (mbi->flags & MB_INFO_CMDLINE)
		printf ("cmdline = %s\n", (char*) mbi->cmdline);

	/* Are mods_* valid? */
	if (mbi->flags & MB_INFO_MODS) {
		module_t *mod;
		int i;

		printf ("mods_count = %d, mods_addr = 0x%x\n",
		    (int)mbi->mods_count, (int)mbi->mods_addr);
		for (i = 0, mod = (module_t*) mbi->mods_addr;
		    i < mbi->mods_count;
		    i++, mod++)
			printf (" mod_start = 0x%x, mod_end = 0x%x, string = %s\n",
			    (unsigned)mod->mod_start,
			    (unsigned)mod->mod_end,
			    (char*) mod->string);
	}
	/* Is the symbol table of a.out valid? */
	if (mbi->flags & MB_INFO_AOUT_SYMS) {
		aout_symbol_table_t *aout_sym = &(mbi->u.aout_sym);

		printf ("aout_symbol_table: tabsize = 0x%0x, "
		    "strsize = 0x%x, addr = 0x%x\n",
		    (unsigned)aout_sym->tabsize,
		    (unsigned)aout_sym->strsize,
		    (unsigned)aout_sym->addr);
	}
	/* Is the section header table of ELF valid? */
	else if (mbi->flags & MB_INFO_ELF_SHDR) {
		elf_section_header_table_t *elf_sec = &(mbi->u.elf_sec);

		printf ("elf_sec: num = %u, size = 0x%x,"
		    " addr = 0x%x, shndx = 0x%x\n",
		    (unsigned)elf_sec->num, (unsigned)elf_sec->size,
		    (unsigned)elf_sec->addr, (unsigned)elf_sec->shndx);
	}
	/* Are mmap_* valid? */
	if (mbi->flags & MB_INFO_MEM_MAP) {
		memory_map_t *mmap;

		printf ("Memory:\n");
		for (mmap = (memory_map_t*) mbi->mmap_addr;
		    (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length;
		    mmap = (memory_map_t*) ((unsigned long)mmap
			+ mmap->size + sizeof (mmap->size))) {
			printf ("  addr = 0x%x%x,"
			    " length = 0x%x%x, type = 0x%x",
			    (unsigned)mmap->base_addr_high,
			    (unsigned)mmap->base_addr_low,
			    (unsigned)mmap->length_high,
			    (unsigned)mmap->length_low,
			    (unsigned)mmap->type);
			if (mmap->type == 1)
				printf (" (RAM)");
			printf ("\n");
		}
	}

	/* Is drive info available? */
	if (mbi->flags & MB_INFO_DRIVE_INFO) {
		drive_info_t *drives;
		unsigned short *port;

		printf ("Drives:\n");
		drives = (drive_info_t*) mbi->drives_addr;
		while ((unsigned long)drives < mbi->drives_addr + mbi->drives_length) {
			printf ("  0x%x (%s), C/H/S = %d/%d/%d, ports =",
			    (unsigned)drives->drive_number,
			    drives->drive_mode == MB_DI_CHS_MODE ? "CHS" : "LBA",
			    (unsigned)drives->drive_cylinders,
			    (unsigned)drives->drive_heads,
			    (unsigned)drives->drive_sectors);
			port = drives->drive_ports;
			while ((unsigned long)port < (unsigned long)drives +
			    drives->size) {
				printf (" 0x%x", *port);
				++port;
			}
			printf ("\n");
			drives = (drive_info_t*) ((unsigned long)drives +
				drives->size + sizeof (drives->size));
		}
	}

	/* ROM config table?
	 * See interrupt 15h/c0h. */
	if ((mbi->flags & MB_INFO_CONFIG_TABLE) && mbi->config_table)
		printf ("ROM config table at 0x%x\n",
			(unsigned)mbi->config_table);

	/* APM table? */
	if (mbi->flags & MB_INFO_APM_TABLE) {
		apm_info_t *apm = (apm_info_t*) mbi->apm_table;

		printf ("APM BIOS Version 0x%x at %x:%x\n",
			(unsigned) apm->version,
			(unsigned) apm->cseg, apm->offset);
	}

	if (mbi->flags & MB_INFO_VIDEO_INFO) {
		/* Video info not implemented in grub 0.97. */
	}
}
