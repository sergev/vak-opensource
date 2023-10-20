//
// Copyright (c) 2023 Serge Vakulenko
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
#include <iostream>
#include <set>
#include <unistd.h>

class Simulator;

class Event {
public:
    virtual void behave(Simulator &sim) = 0;

private:
    uint64_t start_time{};

    // Compare two events by start time and index.
    struct Cmp {
        bool operator()(const Event *a, const Event *b) const
        {
            if (a->start_time < b->start_time)
                return true;
            if (a->start_time > b->start_time)
                return false;
            return (intptr_t)a < (intptr_t)b;
        }
    };

    friend class Simulator;
};

class Simulator {
public:
    bool is_active() const { return !queue.empty(); }

    void wait(Event &event, unsigned num_cycles)
    {
        // Insert event into queue.
        event.start_time = master_clock + num_cycles;
        queue.insert(&event);
    }

    void run()
    {
        // Get next event from queue.
        Event *event = queue.extract(queue.begin()).value(); // C++17

        // Update time.
        if (event->start_time > master_clock) {
            unsigned msec = event->start_time - master_clock;
            usleep(msec * 1000);
        }
        master_clock = event->start_time;

        // Run event.
        event->behave(*this);
    }

    uint64_t get_clock() const { return master_clock; }

private:
    uint64_t master_clock{};

    // Event queue, ordered by start time.
    std::set<Event *, Event::Cmp> queue;
};
