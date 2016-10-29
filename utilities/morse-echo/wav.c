/*
 * Copyright (C) 2005 Serge Vakulenko, <vak@cronyx.ru>
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
#include <string.h>
#include <math.h>
#include "wav.h"

#if defined (__ppc__)
#define LITTLE_ENDIAN_32(x)	((((x) >> 24) & 0xff) | (((x) >> 8) & 0xff00) | \
				 (((x) & 0xff00) << 8) | (((x) & 0xff) << 24))
#define LITTLE_ENDIAN_16(x)	((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))
#endif

#ifndef LITTLE_ENDIAN_32
#define LITTLE_ENDIAN_32(x)	(x)
#endif

#ifndef LITTLE_ENDIAN_16
#define LITTLE_ENDIAN_16(x)	(x)
#endif

struct wav_header_t {
	unsigned long riff;
	unsigned long file_length;
	unsigned long wave;
	unsigned long fmt;
	unsigned long fmt_length;
	short fmt_tag;
	short channels;
	unsigned long sample_rate;
	unsigned long bytes_per_second;
	short block_align;
	short bits;
	unsigned long data;
	unsigned long data_length;
};

/* init with default values */
static struct wav_header_t wavhdr = {
	LITTLE_ENDIAN_32 (0x46464952),		/* "RIFF" */
	0,
	LITTLE_ENDIAN_32 (0x45564157),		/* "WAVE" */
	LITTLE_ENDIAN_32 (0x20746d66),		/* "fmt " */
	LITTLE_ENDIAN_32 (16),
	LITTLE_ENDIAN_16 (1),			/* PCM format */
	LITTLE_ENDIAN_16 (2),
	LITTLE_ENDIAN_32 (44100),
	LITTLE_ENDIAN_32 (192000),
	LITTLE_ENDIAN_16 (4),
	LITTLE_ENDIAN_16 (16),
	LITTLE_ENDIAN_32 (0x61746164),		/* "data" */
	0,
};

static FILE *fp;

void wav_init (char *filename, int sample_rate)
{
	int channels, bits;

	channels = 1;
	bits = 16;

	wavhdr.sample_rate = LITTLE_ENDIAN_32 (sample_rate);
	wavhdr.channels = LITTLE_ENDIAN_16 (channels);
	wavhdr.bits = LITTLE_ENDIAN_16 (bits);
	wavhdr.bytes_per_second = channels * sample_rate * bits / 8;
	wavhdr.bytes_per_second = LITTLE_ENDIAN_32 (wavhdr.bytes_per_second);
	wavhdr.data_length = LITTLE_ENDIAN_32 (0x7ffff000);
	wavhdr.file_length = wavhdr.data_length + sizeof (wavhdr) - 8;

	if (strcmp (filename, "-") == 0)
		fp = stdout;
	else {
		fp = fopen (filename, "wb");
		if (! fp) {
			perror (filename);
			exit (-1);
		}
	}

	/* Reserve space for wave header */
	fwrite (&wavhdr, sizeof (wavhdr), 1, fp);
	wavhdr.data_length = 0;
}

/* close audio device */
void wav_close ()
{
	if (fseek (fp, 0, SEEK_SET) == 0) {
		/* Write wave header */
		wavhdr.file_length = wavhdr.data_length + sizeof (wavhdr) - 8;
		wavhdr.file_length = LITTLE_ENDIAN_32 (wavhdr.file_length);
		wavhdr.data_length = LITTLE_ENDIAN_32 (wavhdr.data_length);
		fwrite (&wavhdr, sizeof (wavhdr), 1, fp);
	}
	fclose (fp);
}

void wav_output (float data)
{
	signed short sample;

	sample = (signed short) (data * 0x7fff);
	sample = LITTLE_ENDIAN_16 (sample);
	fwrite (&sample, sizeof (short), 1, fp);
	wavhdr.data_length += sizeof (short);
}
