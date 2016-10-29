#include <stdio.h>

int main ()
{
    int i;

    for (i=0200; i<0377; i++)
        putchar (i);
    printf ("\n");
    _getch();
    return 0;
}
