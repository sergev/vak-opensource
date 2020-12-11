/*
 * Compute prime numbers up to 67.
 *
 * https://spamsink.dreamwidth.org/1197779.html
 */
#include <stdio.h>
#include <math.h>

double magic(double f)
{
    double ff = floor(f);
    double r = log(ff + 6) * (log(ff + 6) - 1);
    return (f - ff) * floor(r) + f;
}

double C = 2.6358597414547913;

int main()
{
    int i;
    for (i = 1; i < 20; ++i) {
        printf("%d\n", (int)C);
        C = magic(C);
    }
    return 0;
}
