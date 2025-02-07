#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdio.h>

#if DEBUG
#define Debug(__info, ...) printf("Debug: " __info, ##__VA_ARGS__)
#else
#define Debug(__info, ...)
#endif

#endif
