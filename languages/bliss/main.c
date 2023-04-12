#include <stdio.h>

extern void HELLO_WORLD(void);

void PUTS(const char *str)
{
    fputs(str, stdout);
    putchar('\n');
}

int main()
{
    HELLO_WORLD();
}
