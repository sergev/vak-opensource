//
// Demo: two processes.
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
#include <iostream>
#include "simulator.h"

class Demo : public Simulator {
    // Add your stuff here.
};

int main(int argc, char **argv)
{
    Demo sim;

    //
    // Clock generator.
    //
    sim.spawn("clock", [&]() -> Coroutine {
        for (;;) {
            std::cout << '(' << sim.get_clock() << ") Clock set" << std::endl;
            co_await sim.delay(1);

            std::cout << '(' << sim.get_clock() << ") Clock reset" << std::endl;
            co_await sim.delay(1);
        }
    }());

    //
    // Master process.
    //
    sim.spawn("master", [&]() -> Coroutine {

        std::cout << '(' << sim.get_clock() << ") Started" << std::endl;
        co_await sim.delay(5);

        std::cout << '(' << sim.get_clock() << ") Half way through" << std::endl;
        co_await sim.delay(5);

        std::cout << '(' << sim.get_clock() << ") Finished" << std::endl;
        sim.finish();
    }());

    //
    // One shot, with delay.
    //
    sim.spawn("once", [&]() -> Coroutine {
        co_await sim.delay(3);
        std::cout << '(' << sim.get_clock() << ") One shot" << std::endl;
    }());

    // Run simulation until all processes finish.
    while (sim.advance())
        continue;

    return 0;
}
