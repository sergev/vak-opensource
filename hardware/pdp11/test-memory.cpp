#include <verilated.h>		// Defines common routines
#include "Vmemory.h"		// From Verilating "memory.v"
#include "opcode.h"
#if VM_TRACE
# include <verilated_vcd_c.h>	// Trace file format header
#endif

Vmemory *uut;			// Instantiation of module

struct stimulus_1 {
    unsigned addr;
    unsigned data;
};

const struct stimulus_1 tab1[] = {
    {   0,  0x0FF0 },
    {   2,  0x2002 },
    {   4,  0x4004 },
    {   6,  0x6006 },
    {   8,  0x8008 },
    { 0xa,  0xa00a },
    { 0xc,  0xc00c },
    { 0xe,  0xe00e },
    { ~0u,  0      },
};

struct stimulus_2 {
    unsigned addr;
};

const struct stimulus_2 tab2[] = {
    {   6 },
    {   7 },
    {   8 },
    {   9 },
    { 0xa },
    { 0xb },
    { 0xc },
    { 0xd },
    { 0xe },
    { 0xf },
    {   0 },
    {   1 },
    {   2 },
    {   3 },
    {   4 },
    {   5 },
    { ~0u },
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
    uut = new Vmemory;		// Create instance of module

    Verilated::commandArgs (argc, argv);
    Verilated::debug (0);

    uut->addr = 0;              // Initialize inputs
    uut->we = 0;
    uut->clk = 0;
    uut->bytew = 0;
    uut->d_in = 0;

    // Wait for global reset to finish
    delay (5);
    VL_PRINTF ("(%u) started memory testing\n", main_time);

    const struct stimulus_1 *s;
    uut->we = 1;
    for (s=tab1; s->addr != ~0U; s++) {
        delay (5);
        uut->addr = s->addr;
        uut->d_in = s->data;
        delay (5);
        uut->clk = 1;
        delay (5);
        uut->clk = 0;
    }
    uut->we = 0;
    VL_PRINTF ("(%u) memory written, start reading\n", main_time);

    const struct stimulus_2 *t;
    for (t=tab2; t->addr != ~0U; t++) {
        delay (5);
        uut->addr = t->addr;
        delay (5);
        VL_PRINTF ("(%u) %04x - %04x\n", main_time, uut->addr, uut->d_out);
    }
    VL_PRINTF ("(%u) finished memory testing\n", main_time);
    uut->final();

    VL_PRINTF ("All Tests passed\n");
    return 0;
}
