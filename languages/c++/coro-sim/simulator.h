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
#include <coroutine>
#include <cstdint>
#include <string>
#include <memory>

//
// Return type for coroutines.
// With this return type, on the first call of the coroutine function,
// a handle is returned (with void value), right before the body of the function starts.
//
class Coroutine {
public:
    // This structure defines the behavior of the coroutine:
    // (1) suspend initially right before executing the target routine;
    // (2) suspend when done, let caller destroy the handle.
    struct promise_type {
        Coroutine get_return_object() { return { *this }; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        void return_void() noexcept {}
    };

    // Constructor: extract the coroutine handle from promise.
    Coroutine(promise_type &promise)
        : handle(std::coroutine_handle<promise_type>::from_promise(promise))
    {
    }

    // Return the handle, cast to void value.
    operator std::coroutine_handle<>() const { return handle; }

private:
    // Store the coroutine handle here.
    std::coroutine_handle<promise_type> handle;
};

//
// Discrete time simulator based on coroutines.
//
class Simulator {
private:
    //
    // Info about the process.
    //
    struct Process {
        const std::string name;               // Name for log file
        std::coroutine_handle<> continuation; // Handle for coroutine continuation
        uint64_t delay{ 0 };                  // Time to wait
        std::unique_ptr<Process> next;        // The rest of the process queue

        // Allocate a process with given name.
        Process(const std::string &name, std::coroutine_handle<> continuation, std::unique_ptr<Process> next)
            : name(name), continuation(continuation), next(std::move(next))
        {}

        // Destroy the process instance.
        ~Process() { continuation.destroy(); }
    };

    //
    // Info for co_await, to switch from coroutine back to sim.run().
    //
    struct Reschedule {
        constexpr bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<> handle) {}
        constexpr void await_resume() const noexcept {}
    };

    std::unique_ptr<Process> cur_proc;   // Current active process
    std::unique_ptr<Process> proc_queue; // Queue of suspended processes
    uint64_t clock_ticks{ 0 };           // Simulated time

public:
    // Default constructor.
    explicit Simulator() {}

    // Forbid the copy constructor.
    Simulator(const Simulator &) = delete;

    //
    // Return the simulated time.
    //
    uint64_t get_clock() const { return clock_ticks; }

    //
    // Create a process with given name and given top level routine.
    //
    void spawn(const std::string &name, std::coroutine_handle<> continuation);

    //
    // Advance the simulation.
    // Return false when no more processes to run.
    //
    bool advance();

    //
    // Delay the current process by a given number of clock ticks.
    // Return awaitable object.
    // This routine should be invoked as:
    //      co_await sim.delay(N);
    //
    Reschedule delay(uint64_t num_clocks);

    //
    // Finish the simulation.
    //
    void finish() { proc_queue.reset(); }
};
