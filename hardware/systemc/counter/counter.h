//
// This is a 4 bit up-counter with synchronous active high reset and
// with active high enable signal
//
#include "systemc.h"

SC_MODULE (counter_m)
{
    sc_in_clk           clock;          // Clock input of the design
    sc_in<bool>         reset;          // active high, synchronous Reset input
    sc_in<bool>         enable;         // Active high enable signal for counter
    sc_out<sc_uint<4> > counter_out;    // 4 bit vector output of the counter

    // Local variables
    sc_uint<4> count;

    // The actual counter logic
    void incr_count ()
    {
        // At every rising edge of clock we check if reset is active
        // If active, we load the counter output with 4'b0000
        if (reset.read() == 1) {
            count = 0;
            counter_out.write(count);

        // If enable is active, then we increment the counter
        } else if (enable.read() == 1) {
            cout << "(" << sc_time_stamp() << ") Incremented Counter "
                 << count << "\n";
            count += 1;
            counter_out.write(count);
        }
    }

    // Constructor for the counter
    // Since this counter is a positive edge trigged one,
    // We trigger the below block with respect to positive
    // edge of the clock and also when ever reset changes state
    SC_CTOR(counter_m)
    {
        cout << "Executing new" << endl;
        SC_METHOD(incr_count);
        sensitive << reset;
        sensitive << clock.pos();
    }
};
