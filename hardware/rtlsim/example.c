/*
 * Example of simple simulation.
 *
 * Copyright (C) 2013 Serge Vakulenko <serge@vak.ru>
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
#include "rtlsim.h"

/*
 * Signals.
 */
signal_t clock  = signal_init ("clock",  0); /* Main clock of the design */
signal_t reset  = signal_init ("reset",  0); /* Active high, synchronous Reset */
signal_t enable = signal_init ("enable", 0); /* Active high enable signal for counter */
signal_t count  = signal_init ("count",  0); /* 4-bit counter */

/*
 * Clock generator.
 */
void do_clock ()
{
    for (;;) {
        signal_set (&clock, 0);
        process_delay (10);
        signal_set (&clock, 1);
        process_delay (10);
    }
}

/*
 * 4-bit up-counter with synchronous active high reset and
 * with active high enable signal.
 */
void do_counter ()
{
    /* Make a sensitivity list. */
    process_sensitive (&clock, POSEDGE);

    for (;;) {
        /* Wait for event from the sensitivity list. */
        process_wait();

        /* At every rising edge of clock we check if reset is active.
         * If active, we load the counter output with 4'b0000. */
        if (reset.value != 0) {
            signal_set (&count, 0);

        /* If enable is active, then we increment the counter. */
        } else if (enable.value != 0) {
            printf ("(%llu) Incremented Counter %llu\n", time_ticks, count.value);
            signal_set (&count, (count.value + 1) & 15);
        }
    }
}

int main (int argc, char **argv)
{
    /* Create processes with 4kbyte stacks. */
    process_init ("clock", do_clock, 4096);
    process_init ("counter", do_counter, 4096);

    process_delay (100);
    signal_set (&reset, 1);
    printf ("(%llu) Asserting Reset\n", time_ticks);

    process_delay (200);
    signal_set (&reset, 0);
    printf ("(%llu) De-Asserting Reset\n", time_ticks);

    process_delay (100);
    printf ("(%llu) Asserting Enable\n", time_ticks);
    signal_set (&enable, 1);

    process_delay (400);
    printf ("(%llu) De-Asserting Enable\n", time_ticks);
    signal_set (&enable, 0);

    printf ("(%llu) Terminating simulation\n", time_ticks);
    return 0;
}
