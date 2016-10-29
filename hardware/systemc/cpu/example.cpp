#include <systemc.h>
#include "Processor.h"
#include "Memory.h"
using namespace std;

int sc_main(int argc, char* argv[])
{
    try {
        // Instantiate modules
        Memory    mem("memory");
        Processor cpu("cpu");

        // Create wave.vcd trace file.
        sc_trace_file *vcd = sc_create_vcd_trace_file("wave");

        // Signals
        sc_signal<bool>     sig_Read;      // Read request
        sc_signal<bool>     sig_Write;     // Write request
        sc_signal<int>      sig_Addr;      // Address of access [31:2]
        sc_signal<int>      sig_BE;        // Byte enable for access [3:0]
        sc_signal<int>      sig_WData;     // Write data [31:0]
        sc_signal<int>      sig_RData;     // Read data [31:0]
        sc_signal<bool>     sig_Stall;     // Access not ready

        // The clock that will drive the CPU and memory
        sc_clock clk;

        // Connecting module ports with signals
        mem.port_Clk   (clk);
        mem.port_Read  (sig_Read);
        mem.port_Write (sig_Write);
        mem.port_Addr  (sig_Addr);
        mem.port_BE    (sig_BE);
        mem.port_WData (sig_WData);
        mem.port_RData (sig_RData);
        mem.port_Stall (sig_Stall);

        cpu.port_ClkIn (clk);
        cpu.port_Read  (sig_Read);
        cpu.port_Write (sig_Write);
        cpu.port_Addr  (sig_Addr);
        cpu.port_BE    (sig_BE);
        cpu.port_WData (sig_WData);
        cpu.port_RData (sig_RData);
        cpu.port_Stall (sig_Stall);

        // Add signals to trace file
        sc_trace(vcd, clk,       "clk");
        sc_trace(vcd, sig_Read,  "Read");
        sc_trace(vcd, sig_Write, "Write");
        sc_trace(vcd, sig_Addr,  "Addr");
        sc_trace(vcd, sig_BE,    "BE");
        sc_trace(vcd, sig_WData, "WData");
        sc_trace(vcd, sig_RData, "RData");
        sc_trace(vcd, sig_Stall, "Stall");

        cout << "Running (press CTRL+C to exit)... " << endl;

        // Start simulation
        sc_start(100.0, SC_NS);

        // Finalize the trace file.
        sc_close_vcd_trace_file(vcd);
    }
    catch (exception& e) {
        cerr << e.what() << endl;
    }
    return 0;
}
