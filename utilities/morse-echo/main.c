/*
 * Copyright (C) 2005-2012 Serge Vakulenko, <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING" for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include "audio.h"
#include "wav.h"
#include "iir.h"

const char version[] = "1.2";
const char copyright[] = "Copyright (C) 2005-2012 Serge Vakulenko";

char *progname;
int wpm = 12;
int tone = 800;
int space_seen;
int dot_msec;
int dash_msec;
int pause_msec;
int fade_msec = 2;
int sample_rate;
iir_t filter_low;
iir_t filter_high;

void (*output) (float data);

double fade (double sample, int n, int nsamples)
{
	int nfade;

	nfade = fade_msec * sample_rate / 1000;
	if (n < nfade)
		return sample * n / nfade;
	if (n > nsamples - nfade)
		return sample * (nsamples - n) / nfade;
	return sample;
}

void play_sound (int msec, int freq)
{
	int phase;
	int n, nsamples;
	double sample;

	nsamples = msec * sample_rate / 1000;

	phase = 0;
	for (n=0; n<nsamples; ++n) {
		if (freq) {
			phase += freq;
			if (phase > sample_rate)
				phase -= sample_rate;

			sample = 0.6 * sin (2*M_PI * phase/sample_rate);
			sample = fade (sample, n, nsamples);
		} else
			sample = 0;

		sample = iir_filter (&filter_low, sample);
		sample = iir_filter (&filter_high, sample);
		if (sample > 1.0 || sample < -1.0)
			fprintf (stderr, "bad sample %g\n", sample);
		output (sample);
	}
}

void type (char *str)
{
	if (*str++ == '.')
		play_sound (dot_msec, tone);
	else
		play_sound (dash_msec, tone);

	while (*str) {
		play_sound (dot_msec, 0);

		if (*str++ == '.')
			play_sound (dot_msec, tone);
		else
			play_sound (dash_msec, tone);
	}
}

void type_char (char c)
{
	if (isspace (c) || ! isprint (c))
		c = ' ';
	else if (isalpha (c) && islower (c))
		c = toupper (c);

	switch (c) {
	default:
		return;
	case ' ' :
		if (! space_seen)
			play_sound (dot_msec + pause_msec, 0);
		space_seen = 1;
		return;

	case 'A': type (".-"); break;
	case 'B': type ("-..."); break;
	case 'C': type ("-.-."); break;
	case 'D': type ("-.."); break;
	case 'E': type ("."); break;
	case 'F': type ("..-."); break;
	case 'G': type ("--."); break;
	case 'H': type ("...."); break;
	case 'I': type (".."); break;
	case 'J': type (".---"); break;
	case 'K': type ("-.-"); break;
	case 'L': type (".-.."); break;
	case 'M': type ("--"); break;
	case 'N': type ("-."); break;
	case 'O': type ("---"); break;
	case 'P': type (".--."); break;
	case 'Q': type ("--.-"); break;
	case 'R': type (".-."); break;
	case 'S': type ("..."); break;
	case 'T': type ("-"); break;
	case 'U': type ("..-"); break;
	case 'V': type ("...-"); break;
	case 'W': type (".--"); break;
	case 'X': type ("-..-"); break;
	case 'Y': type ("-.--"); break;
	case 'Z': type ("--.."); break;
	case '0': type ("-----"); break;
	case '1': type (".----"); break;
	case '2': type ("..---"); break;
	case '3': type ("...--"); break;
	case '4': type ("....-"); break;
	case '5': type ("....."); break;
	case '6': type ("-...."); break;
	case '7': type ("--..."); break;
	case '8': type ("---.."); break;
	case '9': type ("----."); break;
	case '.': type (".-.-.-"); break;
	case ',': type ("--..--"); break;
	case '?': type ("..--.."); break;
	case '/': type ("-..-."); break;
	case '=': type ("-...-"); break;
	}
	space_seen = 0;
}

void type_string (char *str)
{
	while (*str) {
		type_char (*str++);
		play_sound (pause_msec, 0);
	}

}

void type_file (char *filename)
{
	FILE *fd;
	int c;

	fd = fopen (filename, "r");
	if (! fd) {
		perror (filename);
		exit (-1);
	}
	/* Silence 1 sec at start. */
	play_sound (1000, 0);
	while ((c = getc (fd)) >= 0) {
		type_char (c);
		play_sound (pause_msec, 0);
	}
	/* Silence 1 sec at end. */
	play_sound (1000, 0);
	fclose (fd);
}

void usage ()
{
	fprintf (stderr, "Morse Echo, Version %s, %s\n", version, copyright);
	fprintf (stderr, "Usage:\n");
	fprintf (stderr, "\t%s [options...] word...\n",
		progname);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "\t-w wpm\t\trate, default %d words per minute\n", wpm);
	fprintf (stderr, "\t-e ewpm\t\teffective rate (less than wpm)\n");
	fprintf (stderr, "\t-t tone\t\ttone frequency, default %d Hz\n", tone);
	fprintf (stderr, "\t-f file.txt\tplay text file\n");
	fprintf (stderr, "\t-o file.wav\toutput to WAV file\n");
	exit (-1);
}

int main (int argc, char **argv)
{
	int i, effective_wpm = 0;
	char *infile = 0, *outfile = 0;;

	progname = *argv;
	for (;;) {
		switch (getopt (argc, argv, "w:e:t:f:o:")) {
		case EOF:
			break;
		case 'w':
			wpm = strtol (optarg, 0, 0);
			continue;
		case 'e':
			effective_wpm = strtol (optarg, 0, 0);
			continue;
		case 't':
			tone = strtol (optarg, 0, 0);
			continue;
		case 'f':
			infile = optarg;
			continue;
		case 'o':
			outfile = optarg;
			continue;
		default:
			usage ();
		}
		break;
	}
	argc -= optind;
	argv += optind;

	if ((argc < 1 && ! infile) || wpm == 0 || effective_wpm > wpm)
		usage ();

	/* Compute timings. */
	dot_msec = 1200 / wpm;
	dash_msec = 3600 / wpm;
	if (effective_wpm) {
		/* Inrease pauses to lower the effective wpm. */
		pause_msec = 10000 / effective_wpm - 6400 / wpm;
	} else
		pause_msec = 3600 / wpm;

	/* Open audio device. */
	if (outfile) {
	        sample_rate = 22050;
		wav_init (outfile, sample_rate);
		output = wav_output;
	} else {
		sample_rate = audio_init ();
		output = audio_output;
	}

	/* Low-pass Chebyshev filter at 2205 Hz. */
	iir_init (&filter_low, 6, 9.08615538378e-05,
		0.000545169323027, 0.00136292330757, 0.00181723107676,
		0.00136292330757, 0.000545169323027, 9.08615538378e-05,
		4.4701172094, -8.75559202514, 9.54370751381,
		-6.07937220386, 2.14006030306, -0.324735936703);

	/* Low-pass Chebyshev filter at 1102.5 Hz. */
	/* iir_init (&filter_low, 6, 1.77108957115e-06,
		1.06265374269e-05, 2.65663435672e-05, 3.5421791423e-05,
		2.65663435672e-05, 1.06265374269e-05, 1.77108957115e-06,
		5.33051135452, -11.9661100902, 14.4706671755,
		-9.93770750047, 3.6732816104, -0.570755899549); */

	/* High-pass Chebyshev filter at 220.5 Hz. */
	iir_init (&filter_high, 6, 0.863019610765,
		-5.17811766459, 12.9452941615, -17.2603922153,
		12.9452941615, -5.17811766459, 0.863019610765,
		5.70510190836, -13.5693447532, 17.2223064066,
		-12.3021398595, 4.68921905553, -0.745143105709);

	/* Play file. */
	if (infile)
		type_file (infile);

	/* Play args. */
	for (i=0; i<argc; ++i) {
		space_seen = 1;
		type_string (argv[i]);
		type_char (' ');
	}

	/* Close audio device. */
	if (outfile)
		wav_close ();
	else
		audio_flush ();
	return (0);
}
