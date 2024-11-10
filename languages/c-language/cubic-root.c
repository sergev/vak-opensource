#include <stdio.h>
#include <math.h>
#include <float.h>

double cubic_root(double n)
{
    double x = 1.0; // Initial guess

    for (;;) {
        double nx = ((n / x / x) + x + x) / 3.0;
        if (nx == x) {
            return nx;
        }
        x = nx;
    }
}

void test(double n)
{
#if 1
    printf("cubic root of %.15g\n", n);
    printf("           is %.15g\n", cubic_root(n));
    printf("       expect %.15g\n", cbrt(n));
#else
    printf("cubic root of %a\n", n);
    printf("           is %a\n", cubic_root(n));
    printf("       expect %a\n", cbrt(n));
#endif
}

int main()
{
    test(2.0);
    test(3.0);
    test(4.0);
    test(5.0);
}
