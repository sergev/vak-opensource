/*
 * Decode digital data from SCP input.
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
#include <string.h>
#include <time.h>
#include <err.h>
#include "scp.h"

#define VERSION "1.0"

typedef struct {
    uint64_t nsec;
    uint32_t val;
} event_t;

static event_t *event_tab;
static unsigned nevents;
static unsigned events_allocated;

/*
 * Append event to the list.
 */
static void append_event(uint64_t nsec, unsigned val)
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

static uint64_t last_nsec;

static void print_time(FILE *vcd, uint64_t nsec)
{
    if (nsec > last_nsec) {
        fprintf(vcd, "#%ju\n", (uintmax_t)nsec);
        last_nsec = nsec;
    }
}

typedef struct {
    FILE *vcd;
    int clock_val;          /* 0 or 1 */
    int period;             /* nsec */
    uint64_t last_tick;     /* nsec */
} pll_t;

/*
 * Initialize PLL.
 */
static void pll_init(pll_t *pll, FILE *vcd, int period)
{
    memset(pll, 0, sizeof(*pll));
    pll->vcd = vcd;
    pll->period = period;
    pll->last_tick = 0;
}

/*
 * Push PLL up or down.
 * Adjust period based on input displacement.
 */
static void pll_push(pll_t *pll, int delta)
{
    if (delta > -20 && delta < 20) {
        /* Ignore. */

    } else if (delta < 0 && /*delta > -800 &&*/ pll->period > 1800) {
        pll->period -= 10;
//printf("%8ld: Decrease period %d by %d\n", pll->last_tick/1000, pll->period, 10);

    } else if (delta > 0 && /*delta < 800 &&*/ pll->period < 2200) {
        pll->period += 10;
//printf("%8ld: Increase period %d by %d\n", pll->last_tick/1000, pll->period, 10);

    }
//else printf("%8ld: Bad displacement: period %d, delta %d\n", pll->last_tick/1000, pll->period, delta);
}

static void pll_tick(pll_t *pll, uint64_t nsec)
{
    pll->last_tick = nsec;
    pll->clock_val ^= 1;
    print_time(pll->vcd, nsec);
    fprintf(pll->vcd, "%uc\n", pll->clock_val);
}

/*
 * Update PLL.
 * Generate VCD output for clock.
 */
static void pll_update(pll_t *pll, uint64_t nsec)
{
    if (pll->last_tick == 0) {
        pll_tick(pll, nsec);
        return;
    }

    /* Valid step values are 2, 3 or 4 periods. */
    int64_t step = nsec - pll->last_tick;

    if (step >= pll->period/2 && step <= 3*pll->period/2) {
        /* Step by 1. */
        pll_tick(pll, pll->last_tick + pll->period);
        pll_push(pll, (int)step - pll->period);

    } else if (step >= 3*pll->period/2 && step <= 5*pll->period/2) {
        /* Step by 2. */
        pll_tick(pll, pll->last_tick + pll->period);
        pll_tick(pll, pll->last_tick + pll->period);
        pll_push(pll, (int)step - 2*pll->period);

    } else if (step >= 5*pll->period/2 && step <= 7*pll->period/2) {
        /* Step by 3. */
        pll_tick(pll, pll->last_tick + pll->period);
        pll_tick(pll, pll->last_tick + pll->period);
        pll_tick(pll, pll->last_tick + pll->period);
        pll_push(pll, (int)step - 3*pll->period);

    } else if (step >= 7*pll->period/2 && step <= 9*pll->period/2) {
        /* Step by 4. */
        pll_tick(pll, pll->last_tick + pll->period);
        pll_tick(pll, pll->last_tick + pll->period);
        pll_tick(pll, pll->last_tick + pll->period);
        pll_tick(pll, pll->last_tick + pll->period);
        pll_push(pll, (int)step - 4*pll->period);

    } else if (step >= 9*pll->period/2 && step <= 11*pll->period/2) {
        /* Step by 5. */
        pll_tick(pll, pll->last_tick + pll->period);
        pll_tick(pll, pll->last_tick + pll->period);
        pll_tick(pll, pll->last_tick + pll->period);
        pll_tick(pll, pll->last_tick + pll->period);
        pll_tick(pll, pll->last_tick + pll->period);
        pll_push(pll, (int)step - 5*pll->period);
    } else {
        /* PLL error. */
//printf("%8ld: PLL error: last_tick = %ld, period = %d, step = %ld\n", nsec/1000, pll->last_tick, pll->period, step);
        while (pll->last_tick + pll->period/2 < nsec)
            pll_tick(pll, pll->last_tick + pll->period);
    }
}

/*
 * Write VCD output to the given filename.
 */
void scp_decode_track(scp_file_t *sf, const char *name, int tn, int rev)
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
    if (scp_select_track(sf, tn) < 0)
        errx(1, "Cannot select track %d\n", tn);

    if (rev < 1 || rev > sf->header.nr_revolutions)
        errx(1, "Revolution %d out of range 1...%d\n", rev, sf->header.nr_revolutions);
    rev -= 1;

    fprintf(vcd, "$var wire 1 d track%urev%u $end\n", tn, rev+1);
    fprintf(vcd, "$var wire 1 c clock $end\n");

    fprintf(vcd, "$upscope $end\n");
    fprintf(vcd, "$enddefinitions $end\n");
    fprintf(vcd, "$dumpvars\n");

    /* Collect all data. */
    printf("Collecting SCP input data...\n");
    fflush(stdout);

    scp_reset(sf);

    /* Iterate through all data of this revolution. */
    uint64_t nsec = 0;
    int val = 0;
    append_event(0, 0);
    fprintf(vcd, "0d\n");
    fprintf(vcd, "0c\n");

    do {
        unsigned ticks = scp_next_flux(sf, rev);
        nsec += ticks * 25;
        val ^= 1;
        append_event(nsec, val);

    } while (sf->iter_ptr < sf->iter_limit);

    /* Sort all data by timestamp. */
    printf("Sorting %d samples...\n", nevents);
    fflush(stdout);
    qsort(event_tab, nevents, sizeof(event_tab[0]), event_compare);

    /* Print all data. */
    event_t *ev;
    pll_t pll;
    printf("Generating VCD output...\n");
    fflush(stdout);
    pll_init(&pll, vcd, 2000);
    last_nsec = 0;
    for (ev = event_tab; ev < event_tab + nevents; ev++) {
        if (ev->nsec == 0)
            continue;

        pll_update(&pll, ev->nsec);
        print_time(vcd, ev->nsec);
        fprintf(vcd, "%ud\n", ev->val);
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
