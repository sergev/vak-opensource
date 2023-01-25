#include <stdio.h>

int main ()
{
    if (-5.2 + 4.9 != -0.3) {
        printf("Oops! -0.3\n");
    }
    if (4.8 - 6.1 != -1.3) {
        printf("Oops! -1.3\n");
    }
    if (4.3 - 3.6 != 0.7) {
        printf("Oops! 0.7\n");
    }
}
