// add.hpp — declaration of the add() coroutine.
#pragma once

#include "task.hpp"

// A trivial "async-style" building block: returns a+b as a Task.
Task<int> add(int a, int b);
