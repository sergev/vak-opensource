#!/usr/bin/env python3
#
# Generate C sources for csrr() routine.
#
print("""#include "riscvreg.h"

unsigned csrr(unsigned index)
{
    switch(index) {
    default:
""")

for index in range(0x1000):
    print(f"    case 0x{index:03x}: return csr_read(0x{index:03x});");

print("""
    }
}
""")
