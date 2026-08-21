// add.cpp — the add() coroutine.
//
// The presence of co_return makes this function a coroutine. add() is lazy:
// its body does not run when you *call* it — only when it is co_awaited.

#include <iostream>

#include "add.hpp"

Task<int> add(int a, int b)
{
    std::cout << "    add(" << a << ", " << b << ") is running\n";
    co_return a + b;
}
