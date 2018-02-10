/*
 * Generate VCD waveform.
 *
 * Copyright (C) 2018 Serge Vakulenko
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <err.h>
#include "scp.h"

#define VERSION "1.0"

typedef struct {
    uint64_t nsec;
    uint32_t track;
    uint16_t rev;
    uint16_t val;
} event_t;

static event_t *event_tab;
static unsigned nevents;
static unsigned events_allocated;

/*
 * Append event to the list.
 */
static void append_event(uint64_t nsec, unsigned tn, unsigned rev, unsigned val)
{
    if (nevents >= events_allocated) {
        /* Expand the array. */
        if (events_allocated == 0) {
            events_allocated = 10000;
            event_tab = malloc(events_allocated * sizeof(event_tab[0]));
        } else {
            events_allocated *= 2;
            event_tab = realloc(event_tab, events_allocated * sizeof(event_tab[0]));
        }

        if (!event_tab)
            errx(1, "Not enough memory for VCD file");
    }
    event_t *ev = &event_tab[nevents++];
    ev->nsec = nsec;
    ev->track = tn;
    ev->rev = rev;
    ev->val = val;
}

/*
 * Compare two events by timestamp.
 */
static int event_compare(const void *a, const void *b)
{
    event_t *l = (event_t*) a;
    event_t *r = (event_t*) b;

    if (l->nsec < r->nsec)
        return -1;
    if (l->nsec > r->nsec)
        return 1;
    return 0;
}

/*
 * Write VCD output to the given filename.
 */
void scp_generate_vcd(scp_file_t *sf, const char *name)
{
    FILE *vcd = fopen(name, "w");
    if (! vcd)
        err(1, "%s", name);

    time_t now = 0;
    time(&now);

    fprintf(vcd, "$date\n\t%s$end\n", ctime(&now));
    fprintf(vcd, "$version\n\t%s\n$end\n", VERSION);
    fprintf(vcd, "$timescale\n\t1ns\n$end\n");
    fprintf(vcd, "$scope module scp $end\n");

    /* Every track/rev becomes a wire. */
    int tn;
    for (tn = sf->header.start_track; tn <= sf->header.end_track; tn++) {

        if (scp_select_track(sf, tn) < 0) {
            fprintf(stderr, "Cannot select track %d\n", tn);
            exit(1);
        }

        int rev;
        for (rev = 0; rev < sf->header.nr_revolutions; rev++) {
            fprintf(vcd, "$var wire 1 %c%c%u track%urev%u $end\n",
                'a' + tn/16, 'a' + tn%16, rev+1, tn, rev+1);
        }
    }
    fprintf(vcd, "$upscope $end\n");
    fprintf(vcd, "$enddefinitions $end\n");
    fprintf(vcd, "$dumpvars\n");

    /* Collect all data. */
    printf("Collecting SCP input data...\n");
    fflush(stdout);
    for (tn = sf->header.start_track; tn <= sf->header.end_track; tn++) {

        if (scp_select_track(sf, tn) < 0) {
            fprintf(stderr, "Cannot select track %d\n", tn);
            exit(1);
        }

        int rev;
        for (rev = 0; rev < sf->header.nr_revolutions; rev++) {
            scp_reset(sf);

            /* Iterate through all data of this revolution. */
            uint64_t nsec = 0;
            int val = 0;
            append_event(0, tn, rev, 0);
            fprintf(vcd, "0%c%c%u\n", 'a' + tn/16, 'a' + tn%16, rev + 1);

            do {
                unsigned ticks = scp_next_flux(sf, rev);
                nsec += ticks * 25;
                val ^= 1;
                append_event(nsec, tn, rev, val);

            } while (sf->iter_ptr < sf->iter_limit);
        }
    }

    /* Sort all data by timestamp. */
    printf("Sorting %d samples...\n", nevents);
    fflush(stdout);
    qsort(event_tab, nevents, sizeof(event_tab[0]), event_compare);

    /* Print all data. */
    event_t *ev;
    uint64_t last_nsec = 0;
    printf("Generating VCD output...\n");
    fflush(stdout);
    for (ev = event_tab; ev < event_tab + nevents; ev++) {
// Generate only [10ms, 11ms] range.
//if (ev->nsec >= 11000000) break;
//if (ev->nsec < 10000000) continue;
//ev->nsec -= 10000000;
        if (ev->nsec == 0)
            continue;
        if (ev->nsec > last_nsec) {
            fprintf(vcd, "#%ju\n", (uintmax_t)ev->nsec);
            last_nsec = ev->nsec;
        }
        fprintf(vcd, "%u%c%c%u\n", ev->val, 'a' + (ev->track / 16),
            'a' + (ev->track % 16), ev->rev + 1);
    }

    fprintf(vcd, "$end\n");
    fclose(vcd);
    printf("Done\n");
    fflush(stdout);

    /* Deallocate data. */
    free(event_tab);
    events_allocated = 0;
    nevents = 0;
}
