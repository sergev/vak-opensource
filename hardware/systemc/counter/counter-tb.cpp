#include <systemc.h>
#include "counter.h"

int sc_main (int argc, char* argv[])
{
    sc_signal<bool>   clock;
    sc_signal<bool>   reset;
    sc_signal<bool>   enable;
    sc_signal<sc_uint<4> > counter_out;
    int i = 0;

    // Connect the DUT
    counter_m counter("COUNTER");
    counter.clock(clock);
    counter.reset(reset);
    counter.enable(enable);
    counter.counter_out(counter_out);

    // Open VCD file
    sc_trace_file *wf = sc_create_vcd_trace_file("counter");
    wf->set_time_unit(1, SC_NS);

    // Dump the desired signals
    sc_trace(wf, clock, "clock");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, enable, "enable");
    sc_trace(wf, counter_out, "count");

    // Initialize all variables
    reset = 0;       // initial value of reset
    enable = 0;      // initial value of enable
    for (i=0;i<5;i++) {
        clock = 0;
        sc_start(10, SC_NS);
        clock = 1;
        sc_start(10, SC_NS);
    }
    reset = 1;    // Assert the reset
    cout << "(" << sc_time_stamp() <<") Asserting Reset\n";
    for (i=0;i<10;i++) {
        clock = 0;
        sc_start(10, SC_NS);
        clock = 1;
        sc_start(10, SC_NS);
    }
    reset = 0;    // De-assert the reset
    cout << "(" << sc_time_stamp() <<") De-Asserting Reset\n";
    for (i=0;i<5;i++) {
        clock = 0;
        sc_start(10, SC_NS);
        clock = 1;
        sc_start(10, SC_NS);
    }
    cout << "(" << sc_time_stamp() <<") Asserting Enable\n";
    enable = 1;  // Assert enable
    for (i=0;i<20;i++) {
        clock = 0;
        sc_start(10, SC_NS);
        clock = 1;
        sc_start(10, SC_NS);
    }
    cout << "(" << sc_time_stamp() <<") De-Asserting Enable\n";
    enable = 0; // De-assert enable

    cout << "(" << sc_time_stamp() <<") Terminating simulation\n";
    sc_close_vcd_trace_file(wf);

    // Terminate simulation
    return 0;
}
