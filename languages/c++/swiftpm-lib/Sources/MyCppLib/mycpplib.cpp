#include "mycpplib.h"
#include <stdexcept>

int fibonacci(int n)
{
    if (n < 0) {
        // Negative input not allowed.
        return -1;
    }
    if (n == 0) return 0;
    if (n == 1) return 1;

    int a = 0, b = 1, c = 0;
    for (int i = 2; i <= n; ++i) {
        c = a + b;
        a = b;
        b = c;
    }
    return c;
}
