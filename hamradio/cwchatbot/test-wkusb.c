/*
 * Open USB connection to WinKeyer USB and display
 * bytes of input data stream.
 *
 * Usage:
 *      dump-wkusb /dev/tty.usbserial-A1016UNH
 *
 * Copyright (C) 2012 Serge Vakulenko, <serge@vak.ru>
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
#include <getopt.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>

char *progname;
int verbose;
int trace;
int debug;

extern char *optarg;
extern int optind;

void usage ()
{
    fprintf (stderr, "Dump WinKeyer USB data.\n");
    fprintf (stderr, "Usage:\n\t%s [-vtd] file...\n", progname);
    fprintf (stderr, "Options:\n");
    fprintf (stderr, "\t-v\tverbose mode\n");
    fprintf (stderr, "\t-t\ttrace mode\n");
    fprintf (stderr, "\t-d\tdebug\n");
    exit (-1);
}

int main (int argc, char **argv)
{
    progname = *argv;
    for (;;) {
        switch (getopt (argc, argv, "vtd")) {
        case EOF:
            break;
        case 'v':
            ++verbose;
            continue;
        case 't':
            ++trace;
            continue;
        case 'd':
            ++debug;
            continue;
        default:
            usage ();
        }
        break;
    }
    argc -= optind;
    argv += optind;

    if (argc != 1)
        usage ();

    char *devname = argv[0];

    int fd = open (devname, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        perror (devname);
        exit (1);
    }

    struct termios tio;
    if (tcgetattr (fd, &tio) < 0) {
        perror ("tcgetattr");
        exit (1);
    }

    /*
     * 8 data bits, no parity, no h/w handshake
     * Enable receiver, set local mode, 2 stop bits
     */
    tio.c_cflag &= ~(CSIZE | PARENB | CRTSCTS);
    tio.c_cflag |= CS8 | CLOCAL | CREAD | CSTOPB;

    /* Raw input and output */
    tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tio.c_oflag &= ~OPOST;

    /*
     * Software flow control disabled,
     * do not translate CR to NL.
     */
    tio.c_iflag &= ~(IXON | ICRNL);

    cfsetspeed (&tio, 1200);

    if (tcsetattr (fd, TCSANOW, &tio) < 0) {
        perror ("tcsetattr");
        exit (1);
    }
    usleep (400000);
    printf ("Device: %s\n", devname);

#if 0
    int status;
    ioctl (fd, TIOCMGET, &status);
    status |= TIOCM_DTR;
    status &= ~TIOCM_RTS;
    ioctl (fd, TIOCMSET, &status);
#endif
    int timeout = 60000;
    struct timeval tv;

    tv.tv_sec = timeout/1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    //write (fd, "\x00\x04U", 3);

    /* Host open command. */
    write (fd, "\0\2", 2);

    write (fd, "\0\13", 2);

    for (;;) {
        unsigned char c;
        fd_set rfds;

        FD_ZERO (&rfds);
        FD_SET (fd, &rfds);
        if (select (FD_SETSIZE, &rfds, 0, 0, &tv) <= 0) {
            printf ("Timeout\n");
            continue;
        }

        if (read (fd, &c, 1) != 1) {
            perror ("read");
            exit (1);
        }
        printf ("%02x ", c);
        fflush (stdout);
    }

    return (0);
}
