// fib.cpp — the Fibonacci sequence as a C++20 coroutine.
//
// What makes this function a coroutine is simply the presence of `co_yield`.
// The compiler notices it and rewrites the function so that it can pause at
// each co_yield and resume later, remembering its local variables (a and b)
// across the pauses.

#include "fib.hpp"

Generator<unsigned long long> fib(unsigned long long n)
{
    unsigned long long a = 0;  // current  Fibonacci number
    unsigned long long b = 1;  // next     Fibonacci number

    while (a <= n) {
        co_yield a;             // hand `a` to the caller and pause here

        // When the caller asks for the next value, execution resumes right
        // here, with `a` and `b` exactly as we left them.
        unsigned long long next = a + b;
        a = b;
        b = next;
    }
    // Falling off the end finishes the coroutine (calls promise.return_void()).
}
