/*
 * Read-write configuration memory of FT232R chip.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#include "ftdi.h"

#define VERSION         "1.0"
#define MANUF_FTDI      0x0403

int debug_level;
char *progname;
const char *copyright;
int change_prid;
struct ftdi_context ftdi;

void quit (void)
{
    ftdi_deinit (&ftdi);
}

void interrupted (int signum)
{
    quit();
    _exit (-1);
}

int print_info (int prid)
{
    char manufacturer[64], description[64], serial[64];
    struct ftdi_device_list *all = 0, *lst;
    struct libusb_device_descriptor desc;

    if (ftdi_usb_find_all (&ftdi, &all, MANUF_FTDI, prid) <= 0)
        return -1;

    for (lst=all; lst && lst->dev; lst=lst->next) {
        ftdi_usb_get_strings (&ftdi, lst->dev, manufacturer, sizeof(manufacturer)-1,
            description, sizeof(description)-1, serial, sizeof(serial)-1);
        libusb_get_device_descriptor (lst->dev, &desc);
        printf ("Vendor=%04x, Product=%04x,\n", desc.idVendor, desc.idProduct);
        printf ("    Serial='%.63s', Manufacturer='%.63s', Description='%.63s'\n",
            serial, manufacturer, description);
    }
    ftdi_list_free (&all);
    return 0;
}

void change_product_id (libusb_device *dev, int prid)
{
    int i;
    unsigned char buf [FTDI_DEFAULT_EEPROM_SIZE] = {0};
    unsigned short checksum, value;
    struct ftdi_eeprom eeprom;

    if (ftdi_usb_open_dev (&ftdi, dev) < 0) {
        perror (ftdi.error_str ? ftdi.error_str : "ftdi_usb_open_dev");
        quit ();
	exit (-1);
    }
    if (ftdi_read_eeprom (&ftdi, buf) < 0) {
        perror (ftdi.error_str ? ftdi.error_str : "ftdi_read_eeprom");
fatal:  ftdi_usb_close (&ftdi);
        quit ();
	exit (-1);
    }
    printf ("EEPROM read:\n");
    for (i=0; i<FTDI_DEFAULT_EEPROM_SIZE; i++) {
        printf (" %02x", buf[i]);
        if ((i & 15) == 15) {
                putchar ('\n');
        }
    }
    if (ftdi_eeprom_decode (&eeprom, buf, sizeof(buf)) < 0) {
        fprintf (stderr, "Unable to decode EEPROM block.\n");
        goto fatal;
    }

    /* Set new product ID. */
    buf[0x04] = prid;
    buf[0x05] = prid >> 8;
    printf ("Product ID changed from %04x to %04x.\n",
        eeprom.product_id, prid);
#if 0
    //zero string part
    memset(buf + 0x18, 0, FTDI_DEFAULT_EEPROM_SIZE - 0x18);

    //set offsets
    buf[0x0E] = 0x18;
    buf[0x0F] = strlen(manufacturer) * 2 + 2;

    buf[0x10] = buf[0x0E] + buf[0x0F];
    buf[0x11] = strlen(product) * 2 + 2;

    buf[0x12] = buf[0x10] + buf[0x11];
    buf[0x13] = strlen(serial) * 2 + 2;

    if ((buf[0x12] + buf[0x13]) >= (FTDI_DEFAULT_EEPROM_SIZE - 2)) {
        return -1;
    }

    //write manufacturer
    unsigned char *uc = &buf[buf[0x0E]];
    uc[0] = buf[0x0F]; //length
    uc[1] = 0x03; //string
    uc   += 2;
    for (i=0; manufacturer[i]; i++, uc+=2) {
        *uc = manufacturer[i];
    }

    //write product
    uc = &buf[buf[0x10]];
    uc[0] = buf[0x11]; //length
    uc[1] = 0x03; //string
    uc   += 2;
    for (i=0; product[i]; i++, uc+=2) {
        *uc = product[i];
    }

    //write serial
    uc = &buf[buf[0x12]];
    uc[0] = buf[0x13]; //length
    uc[1] = 0x03; //string
    uc   += 2;
    for (i=0; serial[i]; i++, uc+=2) {
        *uc = serial[i];
    }

    // fix offsets
    buf[0x0E] |= 0x80;
    buf[0x10] |= 0x80;
    buf[0x12] |= 0x80;
#endif
    // repair checksum
    checksum = 0xAAAA;
    for (i=0; i<(FTDI_DEFAULT_EEPROM_SIZE/2)-1; i++) {
        value  = buf[i*2];
        value |= buf[(i*2)+1] << 8;
        checksum = value ^ checksum;
        checksum = (checksum << 1) | (checksum >> 15);
    }
    buf[FTDI_DEFAULT_EEPROM_SIZE - 2] = checksum;
    buf[FTDI_DEFAULT_EEPROM_SIZE - 1] = checksum >> 8;

    //verify new EEPROM
    if (ftdi_eeprom_decode (&eeprom, buf, FTDI_DEFAULT_EEPROM_SIZE) < 0)  {
        fprintf (stderr, "Self-check error.\n");
        goto fatal;
    }

    printf ("EEPROM write:\n");
    for (i=0; i<FTDI_DEFAULT_EEPROM_SIZE; i++) {
        printf (" %02x", buf[i]);
        if ((i & 15) == 15) {
                putchar ('\n');
        }
    }

    if (ftdi_write_eeprom (&ftdi, buf) < 0) {
        perror (ftdi.error_str ? ftdi.error_str : "ftdi_write_eeprom");
        goto fatal;
    }
    printf ("EEPROM successfully written!\n");
    ftdi_usb_close (&ftdi);
}

void do_probe ()
{
    int prid;

    if (ftdi_init (&ftdi) < 0) {
        perror (ftdi.error_str ? ftdi.error_str : "ftdi_init");
        quit ();
	exit (-1);
    }
    for (prid=0x6000; prid <= 0xffff; prid++) {
        if ((prid & 0xf) == 0) {
            printf ("%04x...\r", prid);
            fflush (stdout);
        }
        print_info (prid);
    }
}

void do_info (int prid)
{
    if (ftdi_init (&ftdi) < 0) {
        perror (ftdi.error_str ? ftdi.error_str : "ftdi_init");
        quit ();
	exit (-1);
    }
    if (print_info (prid) < 0) {
        fprintf (stderr, "No device with ID %04x:%04x found.\n", MANUF_FTDI, prid);
        quit ();
	exit (-1);
    }
}

void do_change_product_id (int prid, int prid_new)
{
    struct ftdi_device_list *all = 0, *lst;

    if (ftdi_init (&ftdi) < 0) {
        perror (ftdi.error_str ? ftdi.error_str : "ftdi_init");
        quit ();
	exit (-1);
    }
    if (ftdi_usb_find_all (&ftdi, &all, MANUF_FTDI, prid) <= 0) {
        fprintf (stderr, "No device with ID %04x:%04x found.\n", MANUF_FTDI, prid);
        quit ();
	exit (-1);
    }
    for (lst=all; lst && lst->dev; lst=lst->next) {
        change_product_id (lst->dev, prid_new);
    }
    ftdi_list_free (&all);
}

/*
 * Print copying part of license
 */
static void gpl_show_copying (void)
{
    printf ("%s.\n\n", copyright);
    printf ("This program is free software; you can redistribute it and/or modify\n");
    printf ("it under the terms of the GNU General Public License as published by\n");
    printf ("the Free Software Foundation; either version 2 of the License, or\n");
    printf ("(at your option) any later version.\n");
    printf ("\n");
    printf ("This program is distributed in the hope that it will be useful,\n");
    printf ("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
    printf ("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
    printf ("GNU General Public License for more details.\n");
    printf ("\n");
}

/*
 * Print NO WARRANTY part of license
 */
static void gpl_show_warranty (void)
{
    printf ("%s.\n\n", copyright);
    printf ("BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY\n");
    printf ("FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN\n");
    printf ("OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES\n");
    printf ("PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED\n");
    printf ("OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n");
    printf ("MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS\n");
    printf ("TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE\n");
    printf ("PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,\n");
    printf ("REPAIR OR CORRECTION.\n");
    printf("\n");
    printf ("IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n");
    printf ("WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR\n");
    printf ("REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,\n");
    printf ("INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING\n");
    printf ("OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED\n");
    printf ("TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY\n");
    printf ("YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER\n");
    printf ("PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE\n");
    printf ("POSSIBILITY OF SUCH DAMAGES.\n");
    printf("\n");
}

int main (int argc, char **argv)
{
    int ch, prid, prid_new;
    static const struct option long_options[] = {
        { "help",        0, 0, 'h' },
        { "warranty",    0, 0, 'W' },
        { "copying",     0, 0, 'C' },
        { "version",     0, 0, 'V' },
        { NULL,          0, 0, 0 },
    };

    setvbuf (stdout, (char *)NULL, _IOLBF, 0);
    setvbuf (stderr, (char *)NULL, _IOLBF, 0);
    printf ("Configuration utility for FT232R chip, Version %s.\n", VERSION);
    progname = argv[0];
    copyright = "Copyright (C) 2010 Serge Vakulenko";
    signal (SIGINT, interrupted);
#ifdef __linux__
    signal (SIGHUP, interrupted);
#endif
    signal (SIGTERM, interrupted);

    while ((ch = getopt_long (argc, argv, "DhCVWp",
      long_options, 0)) != -1) {
        switch (ch) {
        case 'D':
            ++debug_level;
            continue;
        case 'h':
            break;
        case 'V':
            /* Version already printed above. */
            return 0;
        case 'C':
            gpl_show_copying ();
            return 0;
        case 'W':
            gpl_show_warranty ();
            return 0;
        case 'p':
            ++change_prid;
            continue;
        }
usage:
        printf ("%s.\n\n", copyright);
        printf ("FT232conf comes with ABSOLUTELY NO WARRANTY; for details\n");
        printf ("use `--warranty' option. This is Open Source software. You are\n");
        printf ("welcome to redistribute it under certain conditions. Use the\n");
        printf ("'--copying' option for details.\n\n");
        printf ("Probe:\n");
        printf ("       ft232conf [product-id]\n");
        printf ("\nChange product ID:\n");
        printf ("       mcprog -p old-product-id new-product-id\n");
        printf ("\nArgs:\n");
        printf ("       -p                  Change product ID\n");
        printf ("       -D                  Debug mode\n");
        printf ("       -h, --help          Print this help message\n");
        printf ("       -V, --version       Print version\n");
        printf ("       -C, --copying       Print copying information\n");
        printf ("       -W, --warranty      Print warranty information\n");
        printf ("\n");
        return 0;
    }
    printf ("%s.\n", copyright);
    argc -= optind;
    argv += optind;

    switch (argc) {
    case 0:
        do_info (0x6001);
        break;
    case 1:
	prid = strtol (argv[0], 0, 16);
        do_info (prid);
        break;
    case 2:
	prid = strtol (argv[0], 0, 16);
	prid_new = strtol (argv[1], 0, 16);
        do_change_product_id (prid, prid_new);
        break;
    default:
        goto usage;
    }
    quit ();
    return 0;
}
