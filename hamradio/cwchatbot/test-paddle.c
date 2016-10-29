/*
 * Display key events from Morse paddle.
 *
 * Usage:
 *      dump-paddle /dev/tty.usbserial-A1016UNH
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
#include <sys/time.h>

#include "paddle.h"

char *progname;

static void sigint (int sig)
{
    paddle_close();
    exit (-1);
}

static int get_msec (struct timeval *oldtv)
{
    struct timeval tv;
    unsigned msec = 0;

    gettimeofday (&tv, 0);
    if (oldtv->tv_sec != 0)
        msec = (int) (tv.tv_sec - oldtv->tv_sec) * 1000 +
               (int) (tv.tv_usec - oldtv->tv_usec + 500) / 1000;
    *oldtv = tv;
    return msec;
}

void usage ()
{
    fprintf (stderr, "Dump Morse paddle state.\n");
    fprintf (stderr, "Usage:\n\t%s [-d] file...\n", progname);
    fprintf (stderr, "Options:\n");
    fprintf (stderr, "\t-d\tdebug\n");
    exit (-1);
}

int main (int argc, char **argv)
{
    progname = *argv;
    for (;;) {
        switch (getopt (argc, argv, "d")) {
        case EOF:
            break;
        case 'd':
            ++paddle_debug;
            continue;
        default:
            usage ();
        }
        break;
    }
    argc -= optind;
    argv += optind;

    if (argc != 0)
        usage ();

    paddle_open();
    atexit (paddle_close);
    signal (SIGINT, sigint);

    int old_daah = -1, old_dit = -1;
    struct timeval tv = {0};
    for (;;) {
        int daah, dit;

        paddle_poll (&daah, &dit);
        if (daah != old_daah || dit != old_dit) {
            int msec = get_msec (&tv);

            printf ("%d - %s\n", msec,
                daah ? "Daah" : dit ? "Dit" : "Idle");
            fflush (stdout);
            old_daah = daah;
            old_dit = dit;
        }
        usleep (10000);
    }
    paddle_close();
    return (0);
}
