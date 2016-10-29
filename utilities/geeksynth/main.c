/*
 * Copyright (C) 2015 Serge Vakulenko, <serge@vak.ru>
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
#include "midi.h"

const char version[] = "1.0";
const char copyright[] = "Copyright (C) 2015 Serge Vakulenko";

char *progname;
int output_rate;

double get_freq()
{
    int new_control = midi.control[KNOB_1];
    static int control;
    static double freq;

    if (control != new_control) {
        control = new_control;
        freq = exp2(control / 12.0) * 4;

        printf("freq = %.1f  \n", freq);
    }
    return freq;
}

double get_ampl()
{
    int new_control = midi.control[SLIDER_1];
    static int control;
    static double ampl;

    if (control != new_control) {
        control = new_control;
        if (control == 0)
            ampl = 0;
        else
            ampl = exp((control - 127) / 24.0);

        printf("ampl = %.2f%%  \n", ampl * 100);
    }
    return ampl;
}

void play_sound()
{
	double phase;
	double sample;

	phase = 0;
	for (;;) {
                double freq = get_freq();
                double ampl = get_ampl();

		if (freq) {
			phase += freq;
			if (phase > output_rate)
				phase -= output_rate;

                        if (midi.control[BUTTON_1]) {
                            /* Sine wave. */
                            sample = ampl * sin(2*M_PI * phase/output_rate);
                        } else {
                            /* Square wave */
                            sample = phase < output_rate/2 ? ampl : -ampl;
                        }
		} else
			sample = 0;

		if (sample > 1.0 || sample < -1.0)
			fprintf(stderr, "bad sample %g\n", sample);
		audio_output(sample);
	}
}

void usage()
{
	fprintf(stderr, "Geek Synth, Version %s, %s\n", version, copyright);
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "    %s [options...]\n",
		progname);
	//fprintf(stderr, "Options:\n");
	//fprintf(stderr, "    -t tone    tone frequency, default 800Hz\n");
	exit(-1);
}

int main(int argc, char **argv)
{
	progname = *argv;
	for (;;) {
		switch (getopt(argc, argv, "")) {
		case EOF:
			break;
		default:
			usage();
		}
		break;
	}
	argc -= optind;
	argv += optind;

	if (argc > 0)
		usage();

	/* Open audio device. */
	output_rate = audio_init();

        midi_init();
        play_sound();

	/* Close audio device. */
	audio_flush();
	return 0;
}
