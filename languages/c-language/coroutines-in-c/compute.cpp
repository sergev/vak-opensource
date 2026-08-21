// compute.cpp — a coroutine that uses co_await to build a result from others.
//
// The presence of co_await / co_return makes compute() a coroutine. It calls
// add() (defined in add.cpp), which is itself a coroutine.

#include <iostream>

#include "add.hpp"
#include "compute.hpp"

Task<int> compute()
{
    std::cout << "compute: about to co_await add(1, 2)\n";
    int x = co_await add(1, 2);   // pauses compute, runs add, resumes with 3
    std::cout << "compute: got x = " << x << "\n";

    std::cout << "compute: about to co_await add(" << x << ", 10)\n";
    int y = co_await add(x, 10);  // pauses again, resumes with 13
    std::cout << "compute: got y = " << y << "\n";

    co_return x + y;              // final result: 16
}
