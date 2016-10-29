#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <CoreAudio/AudioHardware.h>

const char version[] = "1.0";
const char copyright[] = "Copyright (C) 2006 Serge Vakulenko";

char *progname;
int verbose;
int debug;

void usage ()
{
	fprintf (stderr, "Sound Meter, Version %s, %s\n", version, copyright);
	fprintf (stderr, "Usage:\n");
	fprintf (stderr, "\t%s [-vd]\n",
		progname);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "\t-v\tverbose mode\n");
	fprintf (stderr, "\t-d\tdebug\n");
	exit (-1);
}

void audio_list_streams (unsigned long devid, int input_flag)
{
	unsigned long size, n;
	char buf [512];
	AudioBufferList *abl;

	if (AudioDeviceGetPropertyInfo (devid, 0, input_flag,
	    kAudioDevicePropertyStreamConfiguration, &size, 0) != 0) {
		fprintf (stderr, "audio: cannot get stream configuration\n");
		return;
	}
	abl = (AudioBufferList*) buf;
	if (AudioDeviceGetProperty (devid, 0, input_flag,
	    kAudioDevicePropertyStreamConfiguration, &size, abl) != 0) {
		fprintf (stderr, "audio: cannot get stream configuration\n");
		return;
	}
	printf ("buffers = %ld\n", abl->mNumberBuffers);
	for (n=0; n<abl->mNumberBuffers; ++n)
		printf ("   %ld channels = %ld, bytes = %ld\n", n,
			abl->mBuffers[n].mNumberChannels,
			abl->mBuffers[n].mDataByteSize);
	return;
#if 0
	for (n=1; n<99; ++n) {
		printf ("   Channel %ld:\n", n);

		size = sizeof (cname);
		if (AudioDeviceGetProperty (devid, n, input_flag,
		    kAudioDevicePropertyChannelName, &size, &cname) != 0) {
			fprintf (stderr, "audio: cannot get channel name\n");
			break;
		}
		printf ("   Name: %s\n", cname);
	}
#endif
}

void audio_list_devices ()
{
	unsigned long size, devid [10], ndev, n, alive;
	char devname [100], manuf [100];

	size = sizeof (devid);
	if (AudioHardwareGetProperty (kAudioHardwarePropertyDevices,
	    &size, &devid) != 0) {
		fprintf (stderr, "audio: cannot get audio devices\n");
		exit (-1);
	}
	ndev = size / sizeof (devid[0]);
	printf ("Found %ld audio device%s.\n", ndev, ndev==1 ? "" : "s");

	for (n=0; n<ndev; ++n) {
		printf ("\nDevice %ld: id %08lx\n", n, devid [n]);

		size = sizeof (devname);
		if (AudioDeviceGetProperty (devid [n], 0, false,
		    kAudioDevicePropertyDeviceName, &size, &devname) != 0) {
			fprintf (stderr, "audio: cannot get device name\n");
			continue;
		}
		printf ("Name: %s\n", devname);

		size = sizeof (manuf);
		if (AudioDeviceGetProperty (devid [n], 0, false,
		    kAudioDevicePropertyDeviceManufacturer, &size, &manuf) != 0) {
			fprintf (stderr, "audio: cannot get device manufacturer\n");
			continue;
		}
		printf ("Manufacturer: %s\n", manuf);

		size = sizeof (alive);
		if (AudioDeviceGetProperty (devid [n], 0, false,
		    kAudioDevicePropertyDeviceIsAlive, &size, &alive) != 0) {
			fprintf (stderr, "audio: cannot get device activity\n");
			continue;
		}
		printf ("Alive: %s\n", alive ? "Yes" : "No");

		audio_list_streams (devid [n], 0);
		audio_list_streams (devid [n], 1);
	}
}

/*
 * The function that the CoreAudio thread calls when it wants more data
 */
static OSStatus audio_callback (AudioDeviceID device, const AudioTimeStamp *now,
	const AudioBufferList *input_data, const AudioTimeStamp *input_time,
	AudioBufferList *output_data, const AudioTimeStamp *output_time,
	void *client_data)
{
	char *buf;
	static unsigned long bytes;

	buf = (char*) input_data->mBuffers[0].mData;
	bytes += input_data->mBuffers[0].mDataByteSize;
	printf ("\r%ld bytes", bytes);
	fflush (stdout);
	return 0;
}

void audio_init_input ()
{
	unsigned long size, indev, buflen;
	AudioStreamBasicDescription info;

	/* Get default input device id. */
	size = sizeof (indev);
	if (AudioHardwareGetProperty (kAudioHardwarePropertyDefaultInputDevice,
	    &size, &indev) != 0 || indev == kAudioDeviceUnknown) {
		fprintf (stderr, "audio: cannot get input device\n");
		exit (-1);
	}
#if 0
	/* Get input format. */
	size = sizeof (info);
	if (AudioDeviceGetProperty (indev, 0, true,
	    kAudioDevicePropertyStreamFormat, &size, &info) != 0) {
		fprintf (stderr, "audio: cannot get input stream format\n");
		exit (-1);
	}
        printf ("Format: id %08lx, flags %lx\n", info.mFormatID, info.mFormatFlags);
        printf ("Frames per packet: %ld\n", info.mFramesPerPacket);
        printf ("Channels per frame: %ld\n", info.mChannelsPerFrame);
        printf ("Bytes per packet: %ld\n", info.mBytesPerPacket);
        printf ("Bytes per frame: %ld\n", info.mBytesPerFrame);
        printf ("Bits per channel: %ld\n", info.mBitsPerChannel);
	printf ("Sample rate: %g\n", info.mSampleRate);
#endif
	/* Set required input format. */
        info.mFormatID = kAudioFormatLinearPCM;
        info.mFormatFlags = kLinearPCMFormatFlagIsBigEndian |
		kLinearPCMFormatFlagIsPacked | kLinearPCMFormatFlagIsFloat;
	info.mSampleRate = 44100;
        info.mChannelsPerFrame = 2; /* stereo */
        info.mFramesPerPacket = 1;
	info.mBitsPerChannel = 8 * sizeof (float);
	info.mBytesPerPacket = info.mBitsPerChannel / 8 * info.mChannelsPerFrame;
	info.mBytesPerFrame = info.mBytesPerPacket / info.mFramesPerPacket;
	size = sizeof (info);
	if (AudioDeviceSetProperty (indev, 0, 0, true,
	    kAudioDevicePropertyStreamFormat, size, &info) != 0) {
		fprintf (stderr, "audio: cannot setup input stream format\n");
		exit (-1);
	}

	/* Set buffer length. */
	size = sizeof (buflen);
	buflen = info.mBytesPerPacket * 2048;
	if (AudioDeviceSetProperty (indev, 0, 0, true,
	    kAudioDevicePropertyBufferSize, size, &buflen) != 0) {
		fprintf (stderr, "audio: cannot set buffer length\n");
		exit (-1);
	}

	/* Get buffer length. */
	size = sizeof (buflen);
	if (AudioDeviceGetProperty (indev, 0, true,
	    kAudioDevicePropertyBufferSize, &size, &buflen) != 0) {
		fprintf (stderr, "audio: cannot get buffer length\n");
		exit (-1);
	}
	printf ("Buffer length: %ld bytes\n", buflen);

	/* Set the IO proc that CoreAudio will call when it needs data */
	if (AudioDeviceAddIOProc (indev, audio_callback, 0) != 0) {
		fprintf (stderr, "audio: cannot add i/o procedure\n");
		exit (-1);
	}

	/* Start callback */
	if (AudioDeviceStart (indev, audio_callback) != 0) {
		fprintf (stderr, "audio: cannot start device\n");
		exit (-1);
	}
}

int main (int argc, char **argv)
{
/*	int i;*/

	progname = *argv;
	for (;;) {
		switch (getopt (argc, argv, "vd")) {
		case EOF:
			break;
		case 'v':
			++verbose;
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

	if (argc > 0)
		usage ();

	audio_init_input ();
	pause ();
#if 0
	/* Play args. */
	for (i=0; i<argc; ++i) {
		space_seen = 1;
		type_string (argv[i]);
		type_char (' ');
	}
#endif
	return (0);
}
