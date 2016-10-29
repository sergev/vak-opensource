/*
 * Copyright (C) 2005 Serge Vakulenko, <vak@cronyx.ru>
 * Based on libao2 sources.
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
#include <unistd.h>
#include <math.h>
#include <SDL.h>
#include "audio.h"

#define BUFFSIZE	4096
#define NUM_BUFS	16

static unsigned char buffer [NUM_BUFS] [BUFFSIZE];

static unsigned int buf_read = 0;
static unsigned int buf_write = 0;
static unsigned int buf_read_pos = 0;
static unsigned int buf_write_pos = 0;
static int full_buffers = 0;

static unsigned int volume = 127;

/* SDL Callback function */
void audio_callback (void *unused, Uint8 *data, int len)
{
	int offset, x;

	/* SDL_MixAudio (stream, read_buffer (buffers, len), len, SDL_MIX_MAXVOLUME); */
	/* if (! full_buffers)
		printf ("SDL: Buffer underrun!\n"); */
	offset = 0;
	while (len > 0) {
		if (full_buffers == 0)
			break;

		/* no more data buffered! */
		x = BUFFSIZE - buf_read_pos;
		if (x > len)
			x = len;
		memcpy (data + offset, buffer[buf_read] + buf_read_pos, x);
		SDL_MixAudio (data + offset, data + offset, x, volume);
		offset += x;
		len -= x;
		buf_read_pos += x;
		if (buf_read_pos >= BUFFSIZE) {
			/* block is empty, find next! */
			buf_read = (buf_read + 1) % NUM_BUFS;
			--full_buffers;
			buf_read_pos = 0;
		}
	}
	/* printf ("SDL: Full Buffers: %i\n", full_buffers); */
}

int audio_init ()
{
	SDL_AudioSpec aspec, obtained;
	int channels;

	channels = 1;

	/* The desired audio format (see SDL_AudioSpec) */
	aspec.format = AUDIO_S16LSB;

	/* The desired audio frequency in samples-per-second. */
	aspec.freq = 22050;

	/* Number of channels (mono/stereo) */
	aspec.channels = channels;

	/*
	 * The desired size of the audio buffer in samples. This number
	 * should be a power of two, and may be adjusted by the audio driver
	 * to a value more suitable for the hardware. Good values seem to
	 * range between 512 and 8192 inclusive, depending on the application
	 * and CPU speed. Smaller values yield faster response time, but can
	 * lead to underflow if the application is doing heavy processing and
	 * cannot fill the audio buffer in time. A stereo sample consists of
	 * both right and left channels in LR ordering. Note that the number
	 * of samples is directly related to time by the following formula:
	 * ms = (samples*1000)/freq
	 */
	aspec.samples = 1024;

	/*
	 * This should be set to a function that will be called when the
	 * audio device is ready for more data. It is passed a pointer to the
	 * audio buffer, and the length in bytes of the audio buffer. This
	 * function usually runs in a separate thread, and so you should
	 * protect data structures that it accesses by calling SDL_LockAudio
	 * and SDL_UnlockAudio in your code. The callback prototype is: void
	 * callback(void *userdata, Uint8 *stream, int len); userdata is the
	 * pointer stored in userdata field of the SDL_AudioSpec. stream is a
	 * pointer to the audio buffer you want to fill with information and
	 * len is the length of the audio buffer in bytes.
	 */
	aspec.callback = audio_callback;

	/*
	 * This pointer is passed as the first parameter to the callback
	 * function.
	 */
	aspec.userdata = 0;

	/* initialize the SDL Audio system */
	if (SDL_Init (SDL_INIT_AUDIO /* |SDL_INIT_NOPARACHUTE */ )) {
		fprintf (stderr, "audio: initializing SDL failed: %s\n",
			SDL_GetError ());
		exit (-1);
	}
	/* Open the audio device and start playing sound! */
	if (SDL_OpenAudio (&aspec, &obtained) < 0) {
		fprintf (stderr, "audio: unable to open SDL: %s\n",
			SDL_GetError ());
		exit (-1);
	}
	/* did we got what we wanted ? */
	if (obtained.channels != 1 || obtained.freq != aspec.freq ||
	    obtained.format != AUDIO_S16LSB) {
		fprintf (stderr, "audio: unsupported audio format\n");
		exit (-1);
	}
	/* printf ("SDL: buf size = %d\n", obtained.size); */

	/* unsilence audio, if callback is ready */
	SDL_PauseAudio (0);

	/* Reset ring-buffer state */
	buf_read = 0;
	buf_write = 0;
	buf_read_pos = 0;
	buf_write_pos = 0;
	full_buffers = 0;

	return aspec.freq;
}

/* close audio device */
void audio_flush ()
{
	if (buf_write_pos > 0) {
		/* Flush last buffer. */
		memset (buffer[buf_write] + buf_write_pos, 0,
			BUFFSIZE - buf_write_pos);
		++full_buffers;
	}

	/* Wait until playing finishes. */
	while (full_buffers > 0)
		usleep (50000);

	SDL_CloseAudio ();
	SDL_QuitSubSystem (SDL_INIT_AUDIO);
}

void audio_output (float sample)
{
	while (full_buffers >= NUM_BUFS)
		usleep (50000);

	*(short*) (buffer[buf_write] + buf_write_pos) = (short) (sample * 0x7fff);
	buf_write_pos += sizeof (short);

	if (buf_write_pos >= BUFFSIZE) {
		/* block is full, find next! */
		buf_write = (buf_write + 1) % NUM_BUFS;
		++full_buffers;
		buf_write_pos = 0;
	}
}
