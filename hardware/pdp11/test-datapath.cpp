#include <getopt.h>
#include <verilated.h>		// Defines common routines
#include "Vdatapath.h"		// From Verilating "datapath.v"
#include "opcode.h"
#include <verilated_vcd_c.h>	// Trace file format header

Vdatapath *uut;			// Unit under test
VerilatedVcdC *tfp;             // Trace dump

void trace_fetch (unsigned cycount, unsigned addr, unsigned opcode)
{
    unsigned short *mem = uut->v__DOT__ram__DOT__memory + (addr >> 1);

    printf ("%4u-%1u) %06o: %s\n", main_time, cycount, addr,
        disasm (addr, opcode, mem[1], mem[2]));
}

void trace_reg (unsigned cycount, unsigned reg, unsigned val)
{
    static const char *regname[8] = {
        "R0","R1","R2","R3","R4","R5","SP","PC" };

    printf ("%4u-%1u) %s := %06o\n", main_time, cycount,
        regname [reg], val);
}

int main (int argc, char **argv)
{
    for (;;) {
        switch (getopt (argc, argv, "h")) {
        case EOF:
            break;
        default:
usage:      fprintf (stderr, "Usage:\n");
            fprintf (stderr, "        simx [-h] file.out\n");
            fprintf (stderr, "Options:\n");
            fprintf (stderr, "        -h    Print this message\n");
            exit (-1);
        }
        break;
    }
    argc -= optind;
    argv += optind;
    if (argc > 1)
        goto usage;

    uut = new Vdatapath;		// Create instance of module

    Verilated::commandArgs (argc, argv);
    Verilated::debug (0);

#if VM_TRACE                            // If verilator was invoked with --trace
    Verilated::traceEverOn (true);	// Verilator must compute traced signals
    VL_PRINTF ("Enabling waves...\n");
    tfp = new VerilatedVcdC;
    uut->trace (tfp, 99);               // Trace 99 levels of hierarchy
    tfp->open ("datapath_dump.vcd");    // Open the dump file
#endif

    if (argc == 1) {                    // Load executable file
        load_file (argv[0], 0500, uut->v__DOT__ram__DOT__memory);
    }
    VL_PRINTF ("%4u)\tstarted datapath testing\n", main_time);

    uut->reset = 1;                     // Global reset
    uut->clk = 0;
    uut->eval(); main_time++;           // Clock negedge
    uut->clk = 1;
    uut->eval(); main_time++;           // Clock posedge

    uut->reset = 0;                     // Clear reset
    uut->clk = 0;
    VL_PRINTF ("%4u)\tturn reset off\n", main_time);

    // Initiate instruction fetching.
    uut->v__DOT__reg_input = 0500;
    uut->v__DOT__cycount_next = 0;
    uut->v__DOT__ctl_ir_we = 0;

    while (main_time < 200 && ! Verilated::gotFinish()) {
        uut->clk = 1;
	uut->eval();                    // Clock posedge
#if VM_TRACE                            // If verilator was invoked with --trace
	if (tfp)
            tfp->dump (main_time);
#endif
        uut->clk = 0;
	uut->eval();                    // Clock negedge
#if VM_TRACE                            // If verilator was invoked with --trace
	if (tfp)
            tfp->dump (main_time);
#endif
	main_time++;                    // Time passes...
        VL_PRINTF (uut->v__DOT__cycount == 0 ? "------\n" : "\n");
    }
    uut->final();
#if VM_TRACE
    if (tfp) tfp->close();
#endif
    VL_PRINTF ("%4u)\tfinished datapath testing\n", main_time);
    return 0;
}
