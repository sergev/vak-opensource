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
#include <err.h>
#include "scp.h"

/*
 * Flux-based streams
 */
#define CLOCK_CENTRE    2000   /* 2000ns = 2us */
#define CLOCK_MAX_ADJ   10     /* +/- 10% adjustment */
#define CLOCK_MIN(_c)   (((_c) * (100 - CLOCK_MAX_ADJ)) / 100)
#define CLOCK_MAX(_c)   (((_c) * (100 + CLOCK_MAX_ADJ)) / 100)

/*
 * Amount to adjust phase/period of our clock based on each observed flux.
 */
#define PERIOD_ADJ_PCT  5
#define PHASE_ADJ_PCT   60

typedef struct {
    scp_file_t *sf;
    int rev;
    int clock;          /* nsec */
    int flux;           /* nsec */
    int time;           /* nsec */
    int clocked_zeros;
} pll_t;

/*
 * Initialize PLL.
 */
static void pll_init(pll_t *pll, scp_file_t *sf, int rev)
{
    memset(pll, 0, sizeof(*pll));
    pll->sf = sf;
    pll->rev = rev;
    pll->clock = CLOCK_CENTRE;
}

static int pll_next_bit(pll_t *pll)
{
    while (pll->flux < pll->clock/2) {
        pll->flux += 25 * scp_next_flux(pll->sf, pll->rev);
    }

    pll->time += pll->clock;
    pll->flux -= pll->clock;

    if (pll->flux >= pll->clock/2) {
        pll->clocked_zeros++;
        return 0;
    }

    /* PLL: Adjust clock frequency according to phase mismatch.
     * eg. PERIOD_ADJ_PCT=0% -> timing-window centre freq. never changes */
    if (pll->clocked_zeros <= 3) {

        /* In sync: adjust base clock by a fraction of phase mismatch. */
        pll->clock += pll->flux * PERIOD_ADJ_PCT / 100;
    } else {
        /* Out of sync: adjust base clock towards centre. */
        pll->clock += (CLOCK_CENTRE - pll->clock) * PERIOD_ADJ_PCT / 100;
    }

    /* Clamp the clock's adjustment range. */
    if (pll->clock < CLOCK_MIN(CLOCK_CENTRE))
        pll->clock = CLOCK_MIN(CLOCK_CENTRE);
    if (pll->clock > CLOCK_MAX(CLOCK_CENTRE))
        pll->clock = CLOCK_MAX(CLOCK_CENTRE);

    /* PLL: Adjust clock phase according to mismatch.
     * eg. PHASE_ADJ_PCT=100% -> timing window snaps to observed flux. */
    int new_flux = pll->flux * (100 - PHASE_ADJ_PCT) / 100;
    pll->time += pll->flux - new_flux;
    pll->flux = new_flux;

    pll->clocked_zeros = 0;
    return 1;
}

/*
 * Decode MFM data from one track.
 */
void scp_decode_track(scp_file_t *sf, const char *name, int tn, int rev)
{
    pll_t pll;
    int val;

    /* Every track/rev becomes a wire. */
    if (scp_select_track(sf, tn) < 0)
        errx(1, "Cannot select track %d\n", tn);

    if (rev < 1 || rev > sf->header.nr_revolutions)
        errx(1, "Revolution %d out of range 1...%d\n", rev, sf->header.nr_revolutions);

    /* Iterate through all data of this revolution. */
    pll_init(&pll, sf, rev-1);
    scp_reset(sf);
    do {
        val = pll_next_bit(&pll);

        printf("%d", val);
        //TODO

    } while (sf->iter_ptr < sf->iter_limit);
}
