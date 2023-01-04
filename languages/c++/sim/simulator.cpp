//
// Discrete-event simulator based on C++20 coroutines.
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

#include <iostream>

//
// Create a process with given name and given top level routine.
//
void simulator_t::spawn(const std::string &name,
                        std::function<co_void_t(simulator_t &sim)> func,
                        uint64_t delay)
{
    // Allocate new process instance and add to the event queue.
    // Lazy-start the coroutine and store the continuation.
    event_queue = std::make_unique<process_t>(name, func(*this), delay, std::move(event_queue));

    // std::cout << "process " << proc.name << " handle: " << handle.address() << std::endl;
}

//
// Run the simulation.
//
bool simulator_t::advance()
{
    // Select next process from the queue.
    cur_proc = std::move(event_queue);
    if (cur_proc == nullptr) {
        // All done.
        return false;
    }

    // Remove this process from the queue.
    event_queue = std::move(cur_proc->next);

    if (cur_proc->delay != 0) {
        // Advance time.
        time_ticks += cur_proc->delay;
    }

    // Resume the process.
    // std::cout << '(' << time_ticks << ") Resume process '" << cur_proc->name << '\'' <<
    // std::endl;
    cur_proc->continuation.resume();

    // On return, in case the current process is still active - deallocate it.
    if (cur_proc != nullptr) {
        cur_proc.reset();
    }
    return true;
}

//
// Finish the simulation.
//
void simulator_t::finish()
{
    event_queue.reset();
}

//
// Delay the current process by a given number of clock ticks.
// Return awaitable object.
// This routine should be invoked as:
//      co_await sim.delay(N);
//
simulator_t::co_await_t simulator_t::delay(uint64_t num_clocks)
{
    // Put the current process to queue of pending events.
    // Keep the queue sorted.
    std::unique_ptr<process_t> *que_ptr = &event_queue;
    for (;;) {
        process_t *p = que_ptr->get();
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
