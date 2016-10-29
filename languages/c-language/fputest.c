#include <stdio.h>
#include <sys/time.h>

#define N 100000000

double x;

extern double f(double y);

int main ()
{
    double y;
    struct timeval t0, now;
    unsigned usec;

    printf ("--- Test FPU arithmetics ---\n");

    /* Get start time. */
    gettimeofday (&t0, 0);

    /* Make computations. */
    x = 123.456;
    for (y=1; y<=N; y+=1) {
        x = f(y)/2 + y;
    }

    /* Get finish time. */
    gettimeofday (&now, 0);
    usec = (now.tv_sec - t0.tv_sec) * 1000 +
        (now.tv_usec - t0.tv_usec) / 1000;
    if (usec < 1)
        usec = 1;

    printf("      Time: %u usec\n", usec);
    printf("Iterations: %g\n", (double)N);
    printf("     Speed: %.1f Miterations/sec\n", (double)N / usec);
    return 0;
}
