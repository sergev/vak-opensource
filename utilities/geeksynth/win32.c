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
#include <math.h>
#include <windows.h>
#include "audio.h"

#define BUFFER_SIZE  4096
#define BUFFER_COUNT 16

static HWAVEOUT outdev;
static unsigned char buffer [BUFFER_SIZE * BUFFER_COUNT];
static WAVEHDR wave_blocks [BUFFER_COUNT];
static unsigned int buf_index;
static unsigned int buf_write_pos;
static volatile int full_buffers;

static void CALLBACK audio_callback (HWAVEOUT outdev, UINT msg,
	DWORD instance, DWORD param1, DWORD param2)
{
	if (msg != WOM_DONE)
		return;

	if (full_buffers > 0)
		--full_buffers;
}

int audio_init ()
{
	int channels;
	WAVEFORMATEX wformat;
	MMRESULT result;
	int i;

	channels = 1;

	/* fill waveformatex */
	ZeroMemory (&wformat, sizeof (WAVEFORMATEX));
	wformat.cbSize = 0;	/* size of _extra_ info */
	wformat.wFormatTag = WAVE_FORMAT_PCM;
	wformat.nChannels = channels;
	wformat.nSamplesPerSec = 22050;
	wformat.wBitsPerSample = 16; /* 2 bytes per sample */
	wformat.nBlockAlign = wformat.nChannels * wformat.wBitsPerSample / 8;
	wformat.nAvgBytesPerSec = wformat.nSamplesPerSec * wformat.nBlockAlign;

	/* open sound device */
	/* WAVE_MAPPER always points to the default wave device on the system */
	result = waveOutOpen (&outdev, WAVE_MAPPER, &wformat,
		(DWORD_PTR) audio_callback, 0, CALLBACK_FUNCTION);
	if (result == WAVERR_BADFORMAT) {
		fprintf (stderr, "audio: unsupported audio format\n");
		exit (-1);
	}
	if (result != MMSYSERR_NOERROR) {
		fprintf (stderr, "audio: cannot open wave mapper\n");
		exit (-1);
	}

	/* Setup pointers to each buffer */
	for (i=0; i<BUFFER_COUNT; i++) {
		wave_blocks[i].dwBufferLength = BUFFER_SIZE;
		wave_blocks[i].lpData = &buffer [i * BUFFER_SIZE];
	}
	buf_index = 0;
	buf_write_pos = 0;
	full_buffers = 0;

	return wformat.nSamplesPerSec;
}

void audio_flush ()
{
	WAVEHDR *current;

	if (buf_write_pos > 0) {
		/* Flush last buffer. */
		current = &wave_blocks [buf_index];
		current->dwBufferLength = buf_write_pos;
		waveOutPrepareHeader (outdev, current, sizeof (WAVEHDR));
		waveOutWrite (outdev, current, sizeof (WAVEHDR));
		++full_buffers;
	}

	/* Wait until playing finishes. */
	while (full_buffers > 0)
		Sleep (50);

	/* Close audio output device. */
	waveOutReset (outdev);
	waveOutClose (outdev);
}

void audio_output (float sample)
{
	WAVEHDR *current;

	while (full_buffers >= BUFFER_COUNT)
		Sleep (50);

	/* unprepare the header if it is prepared */
	current = &wave_blocks [buf_index];
	if (current->dwFlags & WHDR_PREPARED)
		waveOutUnprepareHeader (outdev, current, sizeof (WAVEHDR));

	*(short*) (current->lpData + buf_write_pos) = (short) (sample * 0x7fff);
	buf_write_pos += sizeof (short);

	if (buf_write_pos >= BUFFER_SIZE) {
		/* prepare header and write data to device */
		waveOutPrepareHeader (outdev, current, sizeof (WAVEHDR));
		waveOutWrite (outdev, current, sizeof (WAVEHDR));

		/* switch to next buffer */
		buf_index = (buf_index + 1) % BUFFER_COUNT;
		buf_write_pos = 0;
		++full_buffers;
	}
}
