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
#include <string.h>
#include <math.h>

#include "keyer.h"
#include "audio.h"

static volatile int daah_active, dit_active;
static volatile int action;
static volatile int idle_count = 6;

static short dit_data [22000];
static short daah_data [44000];
static short pause_data [22000];
static int dit_len;
static int daah_len;

typedef struct {
    const char *character;	/* The character represented */
    const char *representation; /* Dot-dash shape of the character */
} cw_entry_t;

static const cw_entry_t cw_table[] = {
    /* ASCII 7bit letters */
    { "A",   ".-"      }, { "B",    "-..."   }, { "C",   "-.-."   },
    { "D",   "-.."     }, { "E",    "."      }, { "F",   "..-."   },
    { "G",   "--."     }, { "H",    "...."   }, { "I",   ".."     },
    { "J",   ".---"    }, { "K",    "-.-"    }, { "L",   ".-.."   },
    { "M",   "--"      }, { "N",    "-."     }, { "O",   "---"    },
    { "P",   ".--."    }, { "Q",    "--.-"   }, { "R",   ".-."    },
    { "S",   "..."     }, { "T",    "-"      }, { "U",   "..-"    },
    { "V",   "...-"    }, { "W",    ".--"    }, { "X",   "-..-"   },
    { "Y",   "-.--"    }, { "Z",    "--.."   },

    /* Numerals */
    { "0",    "-----"  }, { "1",    ".----"  }, { "2",   "..---"  },
    { "3",    "...--"  }, { "4",    "....-"  }, { "5",   "....."  },
    { "6",    "-...."  }, { "7",    "--..."  }, { "8",   "---.."  },
    { "9",    "----."  },

    /* Punctuation */
    { ".",    ".-.-.-" },
    { ",",    "--..--" },
    { "?",    "..--.." },
    { "/",    "-..-."  },
    { "+",    ".-.-."  },   /* [AR] end of message */
    { "=",    "-...-"  },   /* [BT] break between paragraphs  */

    /* Prosigns */
    { "<SK>", "...-.-" },   /* end of contact */
    { "<KN>", "-.--."  },   /* invite a specific station to transmit */
};

static const int cw_table_size = sizeof(cw_table) / sizeof(cw_entry_t);

/*
 * Decode a morse character.
 */
static const char *decode (int element)
{
    static char word [16];
    static int nbits = 0;
    int i;

    if (element != ' ' && nbits < sizeof(word)-1) {
        /* Append element to the word. */
        word [nbits++] = element;
        //printf ("%c", action);
        return 0;
    }
    if (nbits == 0)
        return 0;

    /* Compare with morse table. */
    word[nbits] = '\0';
    nbits = 0;
    for (i=0; i<cw_table_size; i++) {
        //printf ("comparing sign %d...\n", i);
        if (strcmp (cw_table[i].representation, word) == 0) {
            /* Recognized a valid character. */
            //printf ("[%c]", cw_table[i].character);
            return cw_table[i].character;
        }
    }
    /* Unknown character. */
    //printf ("[??]");
    return "%";
}

/*
 * Fill the array with sinusoidal data
 */
static void fill_data (short *data, int len, int samples_per_cycle)
{
    int n;

    /* First cycle. */
    for (n=0; n<samples_per_cycle; ++n) {
        data[n] = 20000 * sin (2*M_PI*n / samples_per_cycle);
    }

    /* Next cycles. */
    for (; n<len; n+=samples_per_cycle) {
        memcpy (data + n, data, samples_per_cycle * sizeof(data[0]));
    }

    /* Make slow rising and falling edges. */
    int ncycles = 3 * samples_per_cycle;
    double phi = M_PI / ncycles;

    for (n=0; n<ncycles; ++n) {
        double factor = (1 - cos (n * phi)) / 2;

        data [n] = lround (data [n] * factor);
        data [len - 1 - n + ncycles] = lround (data [len - 1 - n] * factor);
    }
}

void keyer_init (int tone, int wpm)
{
    int sample_rate = audio_init();
    int samples_per_cycle = sample_rate / tone / 2 * 2;
    dit_len = sample_rate * 12 / wpm / 10 /
              samples_per_cycle * samples_per_cycle;
    daah_len = 3 * dit_len;
    fill_data (dit_data, dit_len, samples_per_cycle);
    fill_data (daah_data, daah_len, samples_per_cycle);
}

/*
 * Handle a morse element: dit ot daah.
 * Return a decoded character.
 */
const char *keyer_decode (int daah, int dit)
{
    const char *character;

    dit_active = dit;
    daah_active = daah;

    if (action) {
        character = decode (action);
        action = 0;
        return character;
    }
    if (idle_count == 5) {
        /* Inter-word spacing. */
        idle_count++;
        return " ";
    }
    return 0;
}

/*
 * Fill the data array with audio samples.
 * Return the number of samples.
 */
int audio_output (short **data)
{
    if (dit_active) {
        /* Play a dot and one inter-sign interval. */
        action = '.';
        idle_count = 0;
        *data = dit_data;
        return dit_len + dit_len;
    }
    if (daah_active) {
        /* Play a daah and one inter-sign interval. */
        action = '-';
        idle_count = 0;
        *data = daah_data;
        return daah_len + dit_len;
    }
    idle_count++;
    if (idle_count == 1) {
        /* Inter-word interval. */
        action = ' ';
        idle_count++;
        *data = pause_data;
        return dit_len + dit_len;
    }
    /* Pause. */
    *data = pause_data;
    return dit_len;
}
