#include <systemc.h>
#include "Vcounter.h"		// From Verilating "counter.v"

int sc_main (int argc, char* argv[])
{
    Verilated::commandArgs(argc, argv);

    sc_clock            clock ("clock", 20, SC_NS);
    sc_signal<bool>     reset;
    sc_signal<bool>     enable;
    sc_signal<uint32_t> counter_out;

    // Connect the DUT
    Vcounter counter("COUNTER");
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
    reset = 0;                  // initial value of reset
    enable = 0;                 // initial value of enable
    sc_start(100, SC_NS);

    reset = 1;                  // Assert the reset
    cout << "(" << sc_time_stamp() <<") Asserting Reset\n";
    sc_start(200, SC_NS);

    reset = 0;                  // De-assert the reset
    cout << "(" << sc_time_stamp() <<") De-Asserting Reset\n";
    sc_start(100, SC_NS);

    enable = 1;                 // Assert enable
    cout << "(" << sc_time_stamp() <<") Asserting Enable\n";
    sc_start(400, SC_NS);

    enable = 0; // De-assert enable
    cout << "(" << sc_time_stamp() <<") De-Asserting Enable\n";

    cout << "(" << sc_time_stamp() <<") Terminating simulation\n";
    sc_close_vcd_trace_file(wf);

    // Terminate simulation
    return 0;
}
