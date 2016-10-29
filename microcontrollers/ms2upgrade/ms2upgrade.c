/*
 * Upgrade a bootloader of Microstick II.
 *
 * Copyright (C) 2012 Serge Vakulenko
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#include "hidapi.h"

#define VERSION         "1."SVNVERSION
#define FLASH_BASE      0x0001e000
#define FLASH_BYTES     (256 * 1024)

/*
 * Identifiers of USB adapter.
 */
#define MICROCHIP_VID           0x04d8
#define MICROSTICK2_PID         0x8107  /* Microchip Microstick II */
#define PICKIT3_PID             0x900a  /* Microchip PICkit 3 */

/*
 * Macros for converting between hex and binary.
 */
#define NIBBLE(x)       (isdigit(x) ? (x)-'0' : tolower(x)+10-'a')
#define HEX(buffer)     ((NIBBLE((buffer)[0])<<4) + NIBBLE((buffer)[1]))

/* Data to write */
unsigned char flash_data [FLASH_BYTES];
unsigned char cooked_data [FLASH_BYTES];
unsigned flash_nbytes;

int debug_level;
char *progname;
const char copyright[] = "Copyright: (C) 2012 Serge Vakulenko";

hid_device *hiddev;
unsigned char hid_reply [64];

void store_data (unsigned address, unsigned byte)
{
    unsigned offset = address - FLASH_BASE;

    if (offset >= FLASH_BYTES) {
        /* Ignore incorrect data. */
        printf ("%08X: address out of flash memory\n", address);
        return;
    }
    flash_data [offset] = byte;
    if (flash_nbytes <= offset)
        flash_nbytes = offset + 1;
}

/*
 * Read HEX file.
 */
int read_hex (char *filename)
{
    FILE *fd;
    unsigned char buf [256], data[16], record_type, sum;
    unsigned address, high;
    int bytes, i;

    fd = fopen (filename, "r");
    if (! fd) {
        perror (filename);
        exit (1);
    }
    high = 0;
    while (fgets ((char*) buf, sizeof(buf), fd)) {
        if (buf[0] == '\n')
            continue;
        if (buf[0] != ':') {
            fclose (fd);
            return 0;
        }
        if (! isxdigit (buf[1]) || ! isxdigit (buf[2]) ||
            ! isxdigit (buf[3]) || ! isxdigit (buf[4]) ||
            ! isxdigit (buf[5]) || ! isxdigit (buf[6]) ||
            ! isxdigit (buf[7]) || ! isxdigit (buf[8])) {
            printf ("%s: bad HEX record: %s\n", filename, buf);
            exit (1);
        }
	record_type = HEX (buf+7);
	if (record_type == 1) {
	    /* End of file. */
            break;
        }
	if (record_type == 5) {
	    /* Start address, ignore. */
	    continue;
	}

	bytes = HEX (buf+1);
        if (bytes & 1) {
            printf ("%s: odd length\n", filename);
            exit (1);
        }
	if (strlen ((char*) buf) < bytes * 2 + 11) {
            printf ("%s: too short hex line\n", filename);
            exit (1);
        }
	address = high << 16 | HEX (buf+3) << 8 | HEX (buf+5);
        if (address & 3) {
            printf ("%s: odd address\n", filename);
            exit (1);
        }

	sum = 0;
	for (i=0; i<bytes; ++i) {
            data [i] = HEX (buf+9 + i + i);
	    sum += data [i];
	}
	sum += record_type + bytes + (address & 0xff) + (address >> 8 & 0xff);
	if (sum != (unsigned char) - HEX (buf+9 + bytes + bytes)) {
            printf ("%s: bad HEX checksum\n", filename);
            exit (1);
        }

	if (record_type == 4) {
	    /* Extended address. */
            if (bytes != 2) {
                printf ("%s: invalid HEX linear address record length\n",
                    filename);
                exit (1);
            }
	    high = data[0] << 8 | data[1];
	    continue;
	}
	if (record_type != 0) {
            printf ("%s: unknown HEX record type: %d\n",
                filename, record_type);
            exit (1);
        }
        //printf ("%08x: %u bytes\n", address, bytes);
        for (i=0; i<bytes; i++) {
            store_data (address++, data [i]);
        }
    }
    fclose (fd);
    return 1;
}

void interrupted (int signum)
{
    printf ("\nInterrupted.\n");
    _exit (-1);
}

static void pickit_send_buf (unsigned char *buf, unsigned nbytes)
{
    if (debug_level > 0) {
        int k;
        printf ("---Send");
        for (k=0; k<nbytes; ++k) {
            if (k != 0 && (k & 15) == 0)
                printf ("\n       ");
            printf (" %02x", buf[k]);
        }
        printf ("\n");
    }
static unsigned count;
if (count++ > 0) return;
    hid_write (hiddev, buf, 64);
}

static void pickit_send (unsigned argc, ...)
{
    va_list ap;
    unsigned nbytes;
    unsigned char buf [64];

    /* Fill with pad bytes. */
    memset (buf, 0x5B, 64);

    va_start (ap, argc);
    for (nbytes=0; nbytes<argc; ++nbytes)
        buf[nbytes] = va_arg (ap, int);
    va_end (ap);

    pickit_send_buf (buf, nbytes);
}

static void pickit_recv ()
{
    if (hid_read (hiddev, hid_reply, 64) != 64) {
        printf ("Error receiving packet\n");
        exit (-1);
    }
    if (debug_level > 0) {
        int k;
        printf ("--->>>>");
        for (k=0; k<64; ++k) {
            if (k != 0 && (k & 15) == 0)
                printf ("\n       ");
            printf (" %02x", hid_reply[k]);
        }
        printf ("\n");
    }
}

/*
 * Get the command response.
 */
static void get_cmd_response (unsigned cmd)
{
    for (;;) {
        pickit_recv();

        // If the received report does not include the command echo, reject it.
        if (hid_reply[0] != (unsigned char) cmd ||
            hid_reply[1] != (unsigned char) (cmd >> 8))
        {
            printf ("Bad echo of %04x command: %02x%02x\n", cmd, hid_reply[1], hid_reply[0]);
            exit (-1);
        }

        if (hid_reply[2] == 0x00 && hid_reply[3] == 0x00) {
            // Received "Success" indicator.
            break;
        }

        if (hid_reply[2] == 0xFF && hid_reply[3] == 0x00) {
            // Received "Fail" indicator.
            printf ("Command %04x failed\n", cmd);
            exit (-1);
        }

        // The 3rd and 4th byte are either the Success/Fail indicator bytes or the Working
        // Indicator packet.
        if (hid_reply[2] == 0x3F && hid_reply[3] == 0x00) {
            // Received Working Indicator... need to try again for package.
            for (;;) {
                pickit_recv();

                // In subsequent receptions, there is no Command Echo, so everything
                // is shifted to the left by two bytes. The Success or Working Indicator
                // will appear at bytes 0 and 1.

                if (hid_reply[0] == 0x00 && hid_reply[1] == 0x00) {
                    // We received a Success Indicator. Declare success.
                    break;
                }
                if (hid_reply[0] == 0x3F && hid_reply[1] == 0x00) {
                    // We received a Working Indicator. Try again with the next report.
                    continue;
                }
                if (hid_reply[0] == 0xFF && hid_reply[1] == 0x00) {
                    // We received a Fail Indicator. Declare failure.
                    printf ("Command %04x failed\n", cmd);
                    exit (-1);
                }
                // Any other received message is an error.
                printf ("Command %04x failed, unknown reply: %02x%02x\n",
                    cmd, hid_reply[1], hid_reply[0]);
                exit (-1);
            }
            break;
        }

        // If any other value is found, assume failure.
        printf ("Command %04x failed, unknown reply: %02x%02x\n",
            cmd, hid_reply[3], hid_reply[2]);
        exit (-1);
    }
}

static void send_bulk_data (unsigned cooked_nbytes)
{
    static unsigned char buf [FLASH_BYTES];

    // Add the total message length at the end of the first USB report
    int total_nbytes = cooked_nbytes + 2 + 4;

    // Round the packet size up to the nearest USB report size
    if (total_nbytes % 64 != 0)
        total_nbytes = total_nbytes + (64 - total_nbytes % 64);

    // Fill the buffer with our pad value. Useful data will overwrite as necessary.
    memset (buf, 0x5B, total_nbytes);

    if (cooked_nbytes <= 60) {
        // Only one report needed
        memcpy (buf, cooked_data, cooked_nbytes);
    } else {
        // Muliple reports needed
        memcpy (buf, cooked_data, 60);
        memcpy (buf + 64, cooked_data + 60, cooked_nbytes - 60);
    }
    buf[60] = cooked_nbytes + 2;
    buf[61] = (cooked_nbytes + 2) >> 8;
    buf[62] = (cooked_nbytes + 2) >> 16;
    buf[63] = (cooked_nbytes + 2) >> 24;

    /* Compute 16-bit checksum. */
    unsigned sum = 0;
    int i;
    for (i=0; i<cooked_nbytes; i+=2) {
        sum += (cooked_data [i] << 8) | cooked_data [i + 1];
    }
    sum = (~sum) + 1;

    // If the data packet is more than one report big, add 4 to the index to skip
    // over the Bulk Transfer Checksum
    if (total_nbytes > 0x40) {
        buf [cooked_nbytes + 4 + 0] = sum;
        buf [cooked_nbytes + 4 + 1] = sum >> 8;
    } else {
        // The packet is only 1 report, so we don't need to adjust for the DWORD Length
        buf [cooked_nbytes + 0] = sum;
        buf [cooked_nbytes + 1] = sum >> 8;
    }

    for (i=0; i*64 < total_nbytes; i++) {
        pickit_send_buf (buf + i*64, 64);
    }
}

/*
 * Open the device.
 * Print a version of firmware.
 */
void do_probe ()
{
    hiddev = hid_open (MICROCHIP_VID, MICROSTICK2_PID, 0);
    //hiddev = hid_open (MICROCHIP_VID, PICKIT3_PID, 0);
    if (! hiddev) {
        printf ("Microstick II not found.  Check cable connection!\n");
        exit (-1);
    }

    /* Read version of adapter. */
    pickit_send (2, 0x41, 0);           // Get firmware version
    pickit_recv();
    if (hid_reply[0] != 0x41 || hid_reply[1] != 0 ||
        hid_reply[4] != 0 || hid_reply[5] != 0)
    {
        printf ("Bad reply on GETVERSIONS_MPLAB command.\n");
        exit (-1);
    }
    unsigned os_type  = hid_reply[6];
    unsigned os_major = hid_reply[7];
    unsigned os_minor = hid_reply[8];
    unsigned os_rev   = hid_reply[9];
    printf ("Firmware OS: type %02xh, version %d.%d.%d\n",
        os_type, os_major, os_minor, os_rev);

    unsigned ap_type  = hid_reply[10];
    unsigned ap_major = hid_reply[11];
    unsigned ap_minor = hid_reply[12];
    unsigned ap_rev   = hid_reply[13];
    printf ("Firmware Application: type %02xh, version %d.%d.%d\n",
        ap_type, ap_major, ap_minor, ap_rev);
}

void do_program (char *filename, unsigned cmd)
{
    /* Open the device. */
    do_probe ();

    /* Only three of four bytes are actually used. */
    int cooked_nbytes = ((flash_nbytes + 3) / 4) * 3;

    /* Align to 192 - a row size for PIC24F. */
    cooked_nbytes = ((cooked_nbytes + 191) / 192) * 192;

    /* Repack the data. */
    int i, j;
    for (i=j=0; i<flash_nbytes; i+=4, j+=3) {
        cooked_data [j] = flash_data [i];
        cooked_data [j + 1] = flash_data [i + 1];
        cooked_data [j + 2] = flash_data [i + 2];
    }
    printf ("Data: %u bytes\n", cooked_nbytes);

    /*
     * Send the programming command.
     */
    printf ("\nProgram bootloader... ");
    fflush (stdout);

    // Adjust the start address: 2 bytes per program word
    unsigned start_addr = FLASH_BASE / 2;

    // Fill the buffer with pad value.
    unsigned char buf [64];
    memset (buf, 0x5B, 64);

    // Copy the command into the buffer.
    buf[0] = cmd;
    buf[1] = cmd >> 8;

    // Copy the data into the buffer.
    buf[2] = start_addr;
    buf[3] = start_addr >> 8;
    buf[4] = start_addr >> 16;
    buf[5] = start_addr >> 24;
    buf[6] = cooked_nbytes;
    buf[7] = cooked_nbytes >> 8;
    buf[8] = cooked_nbytes >> 16;
    buf[9] = cooked_nbytes >> 24;

    // Fill in the length of the message
    buf[60] = 2 + 8;
    buf[61] = 0;
    buf[62] = 0;
    buf[63] = 0;

    pickit_send_buf (buf, 64);
    get_cmd_response (cmd);

    send_bulk_data (cooked_nbytes);
    pickit_recv();
    if (hid_reply[0] != 0 || hid_reply[1] != 0) {
        printf ("Bad reply after bulk data.\n");
        exit (-1);
    }

    // We got a "success" indicator
    printf ("Done.\n");
}

/*
 * Print copying part of license
 */
static void show_copying (void)
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
static void show_warranty (void)
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

/*
 * Print usage
 */
static void show_usage (void)
{
    printf ("%s.\n\n", copyright);
    printf ("ms2upgrade comes with ABSOLUTELY NO WARRANTY; for details\n");
    printf ("use `--warranty' option. This is Open Source software. You are\n");
    printf ("welcome to redistribute it under certain conditions. Use the\n");
    printf ("'--copying' option for details.\n\n");
    printf ("Probe:\n");
    printf ("       ms2upgrade\n");
    printf ("\nWrite bootloader:\n");
    printf ("       ms2upgrade file.hex\n");
    printf ("\nArgs:\n");
    printf ("       file.hex            Code file in Intel HEX format\n");
    printf ("       -D                  Debug mode\n");
    printf ("       -h, --help          Print this help message\n");
    printf ("       -V, --version       Print version\n");
    printf ("       -C, --copying       Print copying information\n");
    printf ("       -W, --warranty      Print warranty information\n");
    printf ("\n");
}

int main (int argc, char **argv)
{
    int ch;
    static const struct option long_options[] = {
        { "help",        0, 0, 'h' },
        { "warranty",    0, 0, 'W' },
        { "copying",     0, 0, 'C' },
        { "version",     0, 0, 'V' },
        { NULL,          0, 0, 0 },
    };

    setvbuf (stdout, (char *)NULL, _IOLBF, 0);
    setvbuf (stderr, (char *)NULL, _IOLBF, 0);
    printf ("Upgrade bootloader on Microstick II board, Version %s\n", VERSION);
    progname = argv[0];
    signal (SIGINT, interrupted);
#ifdef __linux__
    signal (SIGHUP, interrupted);
#endif
    signal (SIGTERM, interrupted);

    while ((ch = getopt_long (argc, argv, "DhCVW",
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
            show_copying ();
            return 0;
        case 'W':
            show_warranty ();
            return 0;
        }
usage:
        show_usage ();
        return 0;
    }
    printf ("%s\n", copyright);
    argc -= optind;
    argv += optind;

    memset (flash_data, ~0, FLASH_BYTES);

    switch (argc) {
    case 0:
        do_probe ();
        break;
    case 1:
        if (! read_hex (argv[0])) {
            printf ("%s: bad file format\n", argv[0]);
            exit (1);
        }
        do_program (argv[0], 0x23);
        break;
    default:
        goto usage;
    }
    return 0;
}
