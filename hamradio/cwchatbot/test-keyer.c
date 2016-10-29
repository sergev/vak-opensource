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
#include <signal.h>

#include "paddle.h"
#include "audio.h"
#include "keyer.h"

int wpm = 18;
int tone = 800;
char *progname;

static void quit ()
{
    audio_stop();
    paddle_close();
    exit (0);
}

static void sigint (int sig)
{
    quit();
}

void usage ()
{
    fprintf (stderr, "Simple Morse keyer.\n");
    fprintf (stderr, "Usage:\n\t%s [options...]\n", progname);
    fprintf (stderr, "Options:\n");
    fprintf (stderr, "\t-w wpm\t\trate, default %d words per minute\n", wpm);
    fprintf (stderr, "\t-t tone\t\ttone frequency, default %d Hz\n", tone);
    fprintf (stderr, "\t-d\tdebug\n");
    exit (-1);
}

int main (int argc, char **argv)
{
    progname = *argv;
    for (;;) {
        switch (getopt (argc, argv, "dw:t:")) {
        case EOF:
            break;
        case 'w':
            wpm = strtol (optarg, 0, 0);
            continue;
        case 't':
            tone = strtol (optarg, 0, 0);
            continue;
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

    if (argc != 0 || wpm == 0)
        usage ();

    keyer_init (tone, wpm);
    paddle_open();
    atexit (quit);
    signal (SIGINT, sigint);

    printf ("Speed: %d wpm\n", wpm);
    printf ("Tone: %d Hz\n", tone);
    printf ("Keyer ready.\n");
    audio_start();

    for (;;) {
        int daah = 0, dit = 0;
        const char *c;

        paddle_poll (&daah, &dit);
        c = keyer_decode (daah, dit);
        if (c) {
            printf ("%s", c);
            fflush (stdout);
        }
    }
}
