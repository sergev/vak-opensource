/*
 * `timescale 1ns / 1ns
 * module main;
 *
 *     reg [31:0] a, b, sum;
 *     reg [31:0] abuf;
 *
 *     always @(*) abuf <= a;
 *
 *     always @(*) begin
 *         sum = abuf + b;
 *         $display("(%0d) %0d", $time, sum);
 *     end
 *
 *     initial begin
 *         a = 0;
 *         b = 0;
 *         repeat (10) #10 begin
 *             a <= a + 2;
 *             b <= b + 3;
 *         end
 *     end
 * endmodule
 */
#include <stdio.h>
#include "rtlsim.h"

signal_t a    = signal_init ("a",    0);
signal_t b    = signal_init ("b",    0);
signal_t abuf = signal_init ("abuf", 0);
signal_t sum  = signal_init ("sum",  0);

void do_buf ()
{
    process_sensitive (&a, 0);
    for (;;) {
        process_wait();
        //printf ("(%llu) Set abuf = %llu\n", time_ticks, a.value);
        signal_set (&abuf, a.value);
    }
}

void do_sum ()
{
    process_sensitive (&abuf, 0);
    process_sensitive (&b, 0);
    for (;;) {
        process_wait();
        printf ("(%llu) sum = %llu\n", time_ticks, abuf.value + b.value);
        signal_set (&sum, abuf.value + b.value);
    }
}

int main (int argc, char **argv)
{
    process_init ("buf", do_buf, 4096);
    process_init ("sum", do_sum, 4096);

    int i;
    for (i = 0; i < 10; ++i) {
        process_delay (10);
        //printf ("(%llu) Increment a and b\n", time_ticks);
        signal_set (&a, a.value + 2);
        signal_set (&b, b.value + 3);
    }
    process_delay (100);
    //printf ("(%llu) Terminating simulation\n", time_ticks);
    return 0;
}
