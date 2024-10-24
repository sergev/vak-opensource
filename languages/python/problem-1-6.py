#!/usr/bin/env python3
#
# Find a six-digit number that when multiplied by 2, 3, 4, 5, 6
# gives a six-digit number of the same digits.
#
import string

def digits(x):
    d = []
    for c in str(x):
        d.append(c)
    d.sort()
    d.reverse()
    return int("".join(d))

n = 6
for x in range(6*10**(n-1), 10**n, 6):
    d = digits(x)
#   print(d)
    x1 = x // 6
    if digits(x1) != d:
        continue
    x2 = x1 * 2
    if digits(x2) != d:
        continue
    x3 = x1 * 3
    if digits(x3) != d:
        continue
    x4 = x1 * 4
    if digits(x4) != d:
        continue
    x5 = x1 * 5
    if digits(x5) != d:
        continue
    print(x1, x2, x3, x4, x5, x)

# Output:
# 142857 285714 428571 571428 714285 857142
