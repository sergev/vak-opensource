#include <systemc.h>
#include "Memory.h"
using namespace std;

void Memory::execute()
{
    // Initialize memory to some predefined contents.
    for (int i=0; i<MEM_SIZE; i++)
        _data[i] = i + 0xff000;
    port_Stall = true;

    for (;;) {
        wait();

        int addr = port_Addr % MEM_SIZE;

        if (port_Read) {
            // Read word from memory.
            int data = _data[addr];
            port_RData = data;
            port_Stall = false;         // Always ready.

#if defined(PRINT_WHILE_RUN)
            cout << sc_time_stamp() << "\tMemory: Done read request. Addr = " << showbase << hex << addr << ", Data = " << showbase << hex << data << endl;
#endif
            // Hold data until read cleared.
            do {
                wait();
            } while (port_Read);
            port_RData = 0;

        } else if (port_Write) {
            // Write a word to memory.
            int data = port_WData;
            //int be = port_BE;
            port_Stall = true;
#if defined(PRINT_WHILE_RUN)
            cout << sc_time_stamp() << "\tMemory: Started write request. Addr = " << showbase << hex << addr << ", Data = " << showbase << hex << data << endl;
#endif
            wait(10);
            port_Stall = false;

            _data[addr] = data;         // TODO: byte enable
#if defined(PRINT_WHILE_RUN)
            cout << sc_time_stamp() << "\tMemory: Finished write request. Addr = " << showbase << hex << addr << endl;
#endif
            // Wait until write cleared.
            do {
                wait();
            } while (port_Write);
        }
    }
}
