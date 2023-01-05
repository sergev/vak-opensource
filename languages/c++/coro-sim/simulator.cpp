//
// Discrete-time simulator based on C++20 coroutines.
//
// Copyright (c) 2021 Serge Vakulenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "simulator.h"

//
// Create a process with given name and given top level routine.
//
void Simulator::spawn(const std::string &name, std::coroutine_handle<> continuation)
{
    // Allocate new process instance and add to the queue.
    // Lazy-start the coroutine and store the continuation.
    proc_queue = std::make_unique<Process>(name, continuation, std::move(proc_queue));
}

//
// Run the simulation.
//
bool Simulator::advance()
{
    // Select next process from the queue.
    cur_proc = std::move(proc_queue);
    if (cur_proc == nullptr) {
        // All done.
        return false;
    }

    // Remove this process from the queue.
    proc_queue = std::move(cur_proc->next);

    if (cur_proc->delay != 0) {
        // Advance time.
        clock_ticks += cur_proc->delay;
    }

    // Resume the process.
    cur_proc->continuation.resume();

    // On return, when the current process is still active - deallocate it.
    if (cur_proc != nullptr) {
        cur_proc.reset();
    }
    return true;
}

//
// Delay the current process by a given number of clock ticks.
// Return awaitable object.
// This routine should be invoked as:
//      co_await sim.delay(N);
//
Simulator::Reschedule Simulator::delay(uint64_t num_clocks)
{
    // Insert the current process into the queue.
    // Keep the queue sorted.
    std::unique_ptr<Process> *que_ptr = &proc_queue;
    for (;;) {
        Process *p = que_ptr->get();
        if (p == nullptr) {
            break;
        }
        if (p->delay > num_clocks) {
            p->delay -= num_clocks;
            break;
        }

        if (p->delay > 0) {
            num_clocks -= p->delay;
        }
        que_ptr = &p->next;
    }
    cur_proc->delay = num_clocks;
    cur_proc->next = std::move(*que_ptr);
    *que_ptr = std::move(cur_proc);

    // On return, suspend the currect coroutine and switch back to sim.run().
    return {};
}
