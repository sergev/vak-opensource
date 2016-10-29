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
#include <SDL.h>
#include "audio.h"

static short *buffer;
static unsigned int buf_read_pos = 0;
static unsigned int buf_count = 0;

static unsigned int volume = 127;

/*
 * SDL Callback function
 */
void audio_callback (void *unused, Uint8 *data, int buffer_size)
{
	while (buffer_size > 0) {
                if (buf_read_pos >= buf_count) {
                    buf_read_pos = 0;
                    buf_count = audio_output (&buffer);
                    if (buf_count == 0)
                        break;
                }
                int nbytes = (buf_count - buf_read_pos) * sizeof(short);
                if (nbytes > buffer_size)
                    nbytes = buffer_size;

		memcpy (data, buffer + buf_read_pos, nbytes);
		SDL_MixAudio (data, data, nbytes, volume);
		buffer_size -= nbytes;
		buf_read_pos += nbytes / sizeof(short);
		data += nbytes;
	}
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
	if (obtained.format != AUDIO_S16LSB) {
		fprintf (stderr, "audio: unsupported audio format = %u\n",
                        obtained.format);
		exit (-1);
	}
	if (obtained.channels != 1) {
		fprintf (stderr, "audio: single-channel format not supported\n");
		exit (-1);
	}

	//printf ("Audio format: %d (need %d)\n", obtained.format, aspec.format);
	//printf ("        Rate: %d samples/sec\n", obtained.freq);
	//printf ("    Channels: %d\n", obtained.channels);
	//printf (" Buf samples: %d\n", obtained.samples);
	//printf (" Buffer size: %d\n", obtained.size);

	/* Reset ring-buffer state */
	buf_read_pos = 0;
	buf_count = 0;

	return obtained.freq;
}

/*
 * Start audio output.
 */
void audio_start ()
{
	/* unsilence audio, if callback is ready */
	SDL_PauseAudio (0);
}

/*
 * Stop audio output.
 */
void audio_stop ()
{
	SDL_PauseAudio (1);
}
