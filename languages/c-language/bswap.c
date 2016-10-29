#include <stdio.h>
#include <stdint.h>

int bswap16 (int x)
{
    union {
        unsigned short number;
        char byte[2];
    } a, b;

    a.number = x;
    b.byte[1] = a.byte[0];
    b.byte[0] = a.byte[1];
    return b.number;
}

int bswap32(int x)
{
   return __builtin_bswap32(x);
}

long long bswap64(long long x)
{
   return __builtin_bswap64(x);
}

int main ()
{
    printf ("        bswap16 (0x1234)             -> %#x \n",   bswap16 (0x1234));
    printf ("        bswap32 (0x12345678)         -> %#x \n",   bswap32 (0x12345678));
    printf ("        bswap64 (0x1234567890abcdef) -> %#llx \n", bswap64 (0x1234567890abcdefLL));
    return 0;
}
