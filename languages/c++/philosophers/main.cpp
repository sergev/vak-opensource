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
#include "simulator.h"

enum class Fork {
    FREE,
    TAKEN,
};

class Philosopher : public Event {
private:
    const unsigned index;
    Fork &left_fork;
    Fork &right_fork;

    enum class State {
        THINKING,
        EATING,
        WAITING,
    };
    State state{};

public:
    Philosopher(unsigned i, Fork &l, Fork &r)
        : index(i), left_fork(l), right_fork(r) {}

    void behave(Simulator &sim) override
    {
        if (state != State::EATING) {
            // Try to start eating.
            if (left_fork == Fork::FREE && right_fork == Fork::FREE) {
                left_fork  = Fork::TAKEN;
                right_fork = Fork::TAKEN;
                state      = State::EATING;
                sim.wait(*this, 1000);
            } else {
                // Wait for forks.
                state = State::WAITING;
                sim.wait(*this, 10);
            }
        } else {
            // Stop eating.
            left_fork  = Fork::FREE;
            right_fork = Fork::FREE;
            state      = State::THINKING;
            sim.wait(*this, random(10, 2000));
        }
        display(sim.get_clock());
    }

private:
    unsigned random(unsigned lo, unsigned hi)
    {
        return lo + arc4random_uniform(hi - lo + 1);
    }

    void display(uint64_t current_time) const
    {
        static const unsigned xpos[] = { 35, 54, 47, 23, 16 };
        static const unsigned ypos[] = { 3, 10, 20, 20, 10 };
        static bool initialized{};

        if (!initialized) {
            std::cout << "\033[2J"; // Clear screen.
            initialized = true;
        }
        std::cout << "\033[H\033[m" << current_time;
        std::cout << "\033[" << ypos[index] << ';' << xpos[index] << 'H';
        switch (state) {
        case State::THINKING:
            std::cout << "\033[1;34m" << (index + 1) << ": thinking"; // Light blue
            break;
        case State::EATING:
            std::cout << "\033[1;32m" << (index + 1) << ": eating  "; // Light green
            break;
        case State::WAITING:
            std::cout << "\033[1;31m" << (index + 1) << ": waiting "; // Light red
            break;
        }
        std::cout << std::flush;
    }
};

int main()
{
    Fork forks[5]{};
    Philosopher a(0, forks[0], forks[1]);
    Philosopher b(1, forks[1], forks[2]);
    Philosopher c(2, forks[2], forks[3]);
    Philosopher d(3, forks[3], forks[4]);
    Philosopher e(4, forks[4], forks[0]);
    Simulator sim;

    a.behave(sim);
    b.behave(sim);
    c.behave(sim);
    d.behave(sim);
    e.behave(sim);

    while (sim.is_active()) {
        sim.run();
    }
}
