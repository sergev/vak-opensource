// fib.hpp — declaration of the Fibonacci coroutine.
#pragma once

#include "generator.hpp"

// Produces every Fibonacci number that is <= n, one at a time, on demand.
Generator<unsigned long long> fib(unsigned long long n);
