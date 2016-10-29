#include <systemc.h>
#include "Processor.h"
using namespace std;

void Processor::initialize()
{
    port_Read = false;
    port_Write = false;
}

void Processor::step()
{
    bool write_op = rand() & 1;
    int  addr     = rand();

    port_Addr = addr;

    // Activate r/w request.
    if (write_op) {
        int data = rand();

        port_Write = true;
        port_WData = data;
        port_BE = 0xf;

#if defined(PRINT_WHILE_RUN)
        cout << "\n" << sc_time_stamp() << "\tCPU: Sent write request. Addr = " << showbase << hex << addr << ", Data = " << showbase << hex << data << endl;
#endif
        wait();
    } else {
        port_Read = true;

#if defined(PRINT_WHILE_RUN)
        cout << "\n" << sc_time_stamp() << "\tCPU: Sent read request. Addr = " << showbase << hex << addr << endl;
#endif
        wait();
    }

    // Wait until memory ready.
    do {
        wait();
    } while (port_Stall);

    // Clear r/w request.
    if (write_op) {
        port_Write = false;
        port_BE = 0;

#if defined(PRINT_WHILE_RUN)
        cout << sc_time_stamp() << "\tCPU: Write completed.\n";
#endif
    } else {
        int data = port_RData;
        port_Read = false;

#if defined(PRINT_WHILE_RUN)
        cout << sc_time_stamp() << "\tCPU: Received " << showbase << hex << data << " from memory.\n";
#endif
    }
}
