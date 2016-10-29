/*
 * Fast inverse square root.
 * https://gist.github.com/JasonGiedymin/1036533
 */
#include <stdio.h>
#include <stdint.h>

double rsqrt (double x)
{
    double xhalf = 0.5 * x;
    union {
        double float64;
        int64_t int64;
    } u;

    u.float64 = x;
    printf ("  a = %.13a = %.16e\n", u.float64, u.float64);

    u.int64 = 0x5fe6eb50c7aa19f9LL - (u.int64 >> 1);
    printf (" r0 = %.13a = %.16e\n", u.float64, u.float64);

    u.float64 = u.float64 * (1.5 - xhalf * u.float64 * u.float64);
    printf (" r1 = %.13a = %.16e\n", u.float64, u.float64);

    /* This line can be repeated arbitrarily many times to increase accuracy */
    u.float64 = u.float64 * (1.5 - xhalf * u.float64 * u.float64);
    printf (" r2 = %.13a = %.16e\n", u.float64, u.float64);

    u.float64 = u.float64 * (1.5 - xhalf * u.float64 * u.float64);
    printf (" r3 = %.13a = %.16e\n", u.float64, u.float64);

    u.float64 = u.float64 * (1.5 - xhalf * u.float64 * u.float64);
    printf (" r4 = %.13a = %.16e\n", u.float64, u.float64);

    u.float64 = u.float64 * (1.5 - xhalf * u.float64 * u.float64);
    printf (" r5 = %.13a = %.16e\n", u.float64, u.float64);

    return u.float64;
}

int main()
{
    double a = 3.0093288174342917e-36;

    double rsqrt_a = rsqrt (a);

    return rsqrt_a;
}
