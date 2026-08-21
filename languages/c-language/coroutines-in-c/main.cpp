// main.cpp — drives the fib() coroutine and prints the results.

#include <cstdlib>
#include <iostream>

#include "fib.hpp"

int main(int argc, char** argv)
{
    // Upper bound: take it from the command line, or default to 1000.
    unsigned long long limit = 1000;
    if (argc > 1) {
        limit = std::strtoull(argv[1], nullptr, 10);
    }

    std::cout << "Fibonacci numbers up to " << limit << ":\n";

    // Each turn of this loop resumes the coroutine, which runs just far enough
    // to produce ONE number, then pauses again. The numbers are computed
    // lazily, on demand — fib() never builds a container of all of them.
    for (unsigned long long value : fib(limit)) {
        std::cout << value << ' ';
    }
    std::cout << '\n';

    return 0;
}
