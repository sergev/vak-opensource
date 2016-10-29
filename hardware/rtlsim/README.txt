Homebrew RTL simulator
~~~~~~~~~~~~~~~~~~~~~~

Features:
    1) C based;
    2) Event-driven;
    3) Simple data structures;
    4) No dynamic memory allocation;
    5) Using standard setjmp/longjmp() calls for coroutines.

Tested on Linux.

Copyright (C) 2013 Serge Vakulenko <serge@vak.ru>


Values
~~~~~~
    typedef unsigned long long value_t;

value_t is an integer data type of 64-bit width, used for
a time counter and for signal values.  It can be redefined as
32-bit unsigned long for efficiency.


Time
~~~~
    value_t time_ticks;

External variable time_ticks tracks a virtual time of simulation.


Signals and Nets
~~~~~~~~~~~~~~~~
signal_t -- a hardware logic signal, or net, with width from 1 to 64 bits.
Roughly equivalent to Verilog reg datatype.  Defined as C data structure:

    typedef struct {
        ...
        const char *name;
        value_t     value;
        ...
    } signal_t;

The user program should treat the name and value fields as read-only.

Signals are declared and initialized as static variables using signal_init()
macro.  A name and an initial value for a signal is specified, for example:

    signal_t clock = signal_init ("clock",  0);

To change a value of a signal, signal_set() function should be used.
It is equivalent to non-blocking assignment in Verilog:

    signal_set (&clock, 1);


Processes
~~~~~~~~~
process_t -- a lightweight thread, modelling a behavior of some hardware unit.
Implemented using standard setjmp/longjmp() functions.
The process is equivalent to 'always' block in Verilog.
Defined as C data structure:

    typedef struct {
        ...
        const char *name;
        ...
    } process_t;

The user program should treat the name and value fields as read-only.

Every process has it's own stack, allocated from the stack of a parent
process.  Usually, processes are created by main() routine at start,
so the default program's stack is used.  When process is created,
a name, a start routine and a stack size is specified.  For example:

    process_init ("counter", do_counter, 4096);

Here, do_counter() is a start routine of the process.
Stack size of 4 kbytes is usually enough for simple processes.
A routine of a processes typically follows a common pattern:
it sets a sensitivity list and goes into an infinite loop,
waiting for event and then assigning new values to output signals.
For example:

    void do_counter()
    {
        process_sensitive (&clock, POSEDGE);
        for (;;) {
            process_wait();
            ...
            signal_set (&count, count.value + 1);
            ...
        }
    }

Function process_sensitive(signal, edge) adds a signal and edge option
to a sensitivity list of the current process.  It allocates a small chunk
of data from the current stack (via alloca).  Any number of signals can
be added to a sensitivity list.  Edge flag can be POSEDGE, NEGEDGE
or 0 (any edge).

Function process_wait() suspends the process until any of signals
in the sensitivity list chenged it's value and matches the edge condition.

Function process_wait(ticks) suspends the current process for
a specified amount of simulated time.

Global variable:

    process_t *process_current;

points to the current running process.  Field process_current->name
can be used for trace log messages.

An example of the simulation you can find in file example.c.
