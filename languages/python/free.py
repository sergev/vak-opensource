#!/usr/bin/env python3
#
# Print memory status like 'free' command in Linux:
#
#                total        used        free      shared  buff/cache   available
# Mem:         4001412      303916      211000       12232     3486496     3397544
# Swap:        4194300       25344     4168956
#
# Use "pip3 install psutil".
#
import psutil

def mbytes(bytes):
    return (bytes + 512*1024) // 1024 // 1024

def main():
    print("     Mbytes:  total        used        free      active    inactive   available")

    #
    # Get memory status:
    # svmem(total=17179869184, available=7341481984, percent=57.3, used=9451421696,
    #       free=440045568, active=6907736064, inactive=6577897472, wired=2543685632)
    #
    mem = psutil.virtual_memory()
    buff_cache = mem.available + mem.used - mem.free
    print(f"Mem:   {mbytes(mem.total):>12d}{mbytes(mem.used):>12d}\
{mbytes(mem.free):>12d}{mbytes(mem.active):>12d}{mbytes(mem.inactive):>12d}{mbytes(mem.available):>12d}")

    #
    # Get swap status:
    # sswap(total=0, used=0, free=0, percent=0.0, sin=75663450112, sout=145195008)
    #
    swap = psutil.swap_memory()
    print(f"Swap:  {mbytes(swap.total):>12d}{mbytes(swap.used):>12d}{mbytes(swap.free):>12d}")

if __name__ == '__main__':
    main()
