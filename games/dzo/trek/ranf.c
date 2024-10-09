#include <stdio.h>
#include <stdlib.h>

#include "trek.h"

int ranf(int max)
{
    register int t;

    if (max <= 0)
        return (0);
    t = rand() >> 5;
    return (t % max);
}

FLOAT franf()
{
    FLOAT t;
    t = rand() & 077777;
    return (t / 32767.0);
}
