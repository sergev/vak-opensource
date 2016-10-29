#include <verilated.h>		// Defines common routines
#include "Vregfile.h"		// From Verilating "regfile.v"
#include "opcode.h"
#if VM_TRACE
# include <verilated_vcd_c.h>	// Trace file format header
#endif

Vregfile *uut;			// Instantiation of module

struct stimulus_1 {
    unsigned selb;
    unsigned w;
};

const struct stimulus_1 tab1[] = {
    { 0,  0xAAAA },
    { 1,  0x1111 },
    { 2,  0x2222 },
    { 3,  0x3333 },
    { 4,  0x4444 },
    { 5,  0x5555 },
    { 6,  0x6666 },
    { 7,  0x7777 },
    { ~0, 0     },
};

struct stimulus_2 {
    unsigned sela;
    unsigned selb;
};

const struct stimulus_2 tab2[] = {
    { 1,  3 },
    { 2,  4 },
    { 3,  5 },
    { 4,  6 },
    { 5,  7 },
    { 6,  0 },
    { 7,  1 },
    { 0,  2 },
    { ~0, 0 },
};

static void delay (unsigned n)
{
    unsigned t = main_time + n;
    while (main_time != t && !Verilated::gotFinish()) {
	uut->eval();            // Evaluate model
	main_time++;		// Time passes...
    }
}

int main (int argc, char **argv)
{
    uut = new Vregfile;		// Create instance of module

    Verilated::commandArgs (argc, argv);
    Verilated::debug (0);

    uut->sela = 0;              // Initialize inputs
    uut->selb = 0;
    uut->we = 0;
    uut->clk = 0;
    uut->w = 0;

    // Wait for global reset to finish
    delay (5);
    VL_PRINTF ("(%u) started regfile testing\n", main_time);

    const struct stimulus_1 *s;
    uut->we = 1;
    for (s=tab1; s->selb != ~0U; s++) {
        delay (5);
        uut->selb = s->selb;
        uut->w = s->w;
        delay (5);
        uut->clk = 1;
        delay (5);
        uut->clk = 0;
    }
    uut->we = 0;
    VL_PRINTF ("(%u) registers written, start reading\n", main_time);

    const struct stimulus_2 *t;
    uut->we = 1;
    for (t=tab2; t->sela != ~0U; t++) {
        delay (5);
        uut->sela = t->sela;
        uut->selb = t->selb;
        delay (5);
        VL_PRINTF ("(%u) %x - %04x // %x - %04x\n",
            main_time, uut->sela, uut->a, uut->selb, uut->b);
    }
    VL_PRINTF ("(%u) finished regfile testing\n", main_time);
    uut->final();

    VL_PRINTF ("All Tests passed\n");
    return 0;
}
