#!/usr/bin/env python3

from itertools import product, chain

s = list(map(str, range(2, 10)))
for x in product(*[["", "|", "&", "^"]] * len(s)):
    expr = "1" + ''.join(chain.from_iterable(zip(x, s)))
    if eval(expr) == 100:
        print(expr)
