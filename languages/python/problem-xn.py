#!/usr/bin/env python3
#
# Find the minimum natural number that increases N times after
# moving its last digit N to the beginning of the number.
#
for n in range(2, 10):
    for k in range(1, 100):
        x = n * (10**k - n)
        if x % (10*n - 1) == 0:
            v = 10 * x // (10*n - 1) + n
            print("For", n, "solution =", v)
            break
