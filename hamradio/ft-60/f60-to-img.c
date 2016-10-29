#include <stdio.h>

void decode (char *buf)
{
    int i, x;

    for (i=0; i<16; i++) {
        switch (i) {
        default:
        case 0: x = (i << 4) + 1;       break;
        case 1: x = (i << 4) + 0x81;    break;
        case 2: x = (i << 4) + 1;       break;
        case 1: x = (i << 4) + 0x83;    break;
        }
        buf[i] ^= x;
    }
}

int main ()
{
    int nbytes;
    char buf[16];

    for (;;) {
        nbytes = read (0, buf, 16);
        if (nbytes <= 0)
            break;
        decode (buf);
        write (1, buf, nbytes);
        if (nbytes < 16)
            break;
    }
    return 0;
}
