/*
 * Simple sound playback using ALSA API and libasound.
 *
 * Compile:
 * $ cc -o play sound_playback.c -lasound
 *
 * Usage:
 * $ ./play <sample_rate> <channels> <seconds> < <file>
 *
 * Examples:
 * $ ./play 44100 2 5 < /dev/urandom
 * $ ./play 22050 1 8 < /path/to/file.wav
 *
 * Copyright (C) 2009 Alessandro Ghedini <alessandro@ghedini.me>
 * --------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Alessandro Ghedini wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we
 * meet some day, and you think this stuff is worth it, you can
 * buy me a beer in return.
 * --------------------------------------------------------------
 */

#include <alsa/asoundlib.h>
#include <stdio.h>

#define PCM_DEVICE "default"

int main(int argc, char **argv)
{
    unsigned int err, tmp, rate;
    int channels, seconds;
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames;
    char *buff;
    int buff_size, loops;

    if (argc < 4) {
        printf("Usage: %s <sample_rate> <channels> <seconds>\n",
                                argv[0]);
        return -1;
    }

    rate     = atoi(argv[1]);
    channels = atoi(argv[2]);
    seconds  = atoi(argv[3]);

    /* Open the PCM device in playback mode */
    err = snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0)
        printf("ERROR: Can't open \"%s\" PCM device. %s\n",
                    PCM_DEVICE, snd_strerror(err));

    /* Allocate parameters object and fill it with default values*/
    snd_pcm_hw_params_alloca(&params);

    snd_pcm_hw_params_any(pcm_handle, params);

    /* Set parameters */
    err = snd_pcm_hw_params_set_access(pcm_handle, params,
        SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0)
        printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(err));

    err = snd_pcm_hw_params_set_format(pcm_handle, params,
        SND_PCM_FORMAT_S16_LE);
    if (err < 0)
        printf("ERROR: Can't set format. %s\n", snd_strerror(err));

    err = snd_pcm_hw_params_set_channels(pcm_handle, params, channels);
    if (err < 0)
        printf("ERROR: Can't set channels number. %s\n", snd_strerror(err));

    err = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0);
    if (err < 0)
        printf("ERROR: Can't set rate. %s\n", snd_strerror(err));

    /* Write parameters */
    err = snd_pcm_hw_params(pcm_handle, params);
    if (err < 0)
        printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(err));

    /* Resume information */
    printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));

    printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

    snd_pcm_hw_params_get_channels(params, &tmp);
    printf("channels: %i ", tmp);

    if (tmp == 1)
        printf("(mono)\n");
    else if (tmp == 2)
        printf("(stereo)\n");

    snd_pcm_hw_params_get_rate(params, &tmp, 0);
    printf("rate: %d bps\n", tmp);

    printf("seconds: %d\n", seconds);

    /* Allocate buffer to hold single period */
    snd_pcm_hw_params_get_period_size(params, &frames, 0);

    buff_size = frames * channels * 2 /* 2 -> sample size */;
    buff = (char *) malloc(buff_size);

    snd_pcm_hw_params_get_period_time(params, &tmp, NULL);

    for (loops = (seconds * 1000000) / tmp; loops > 0; loops--) {

        err = read(0, buff, buff_size);
        if (err == 0) {
            printf("Early end of file.\n");
            return 0;
        }

        err = snd_pcm_writei(pcm_handle, buff, frames);
        if (err == -EPIPE) {
            printf("XRUN.\n");
            snd_pcm_prepare(pcm_handle);
        } else if (err < 0) {
            printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(err));
        }

    }

    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    free(buff);

    return 0;
}
