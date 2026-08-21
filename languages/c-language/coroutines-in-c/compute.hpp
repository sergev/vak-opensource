// compute.hpp — declarations for the co_await demo coroutines.
#pragma once

#include "task.hpp"

// Composes several add() calls using co_await, returning the final result.
Task<int> compute();
