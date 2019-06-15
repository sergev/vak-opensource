//
// Numeric vector sort
//
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <sys/time.h>

double clock_now()
{
    struct timeval now;

    gettimeofday(&now, NULL);
    return (double)now.tv_sec + (double)now.tv_usec/1.0e6;
}

void quicksort(double *a, int lo, int hi)
{
    int i = lo;
    int j = hi;
    while (i < hi) {
        double pivot = a[(lo+hi)/2];

        // Partition
        while (i <= j) {
            while (a[i] < pivot) {
                i = i + 1;
            }
            while (a[j] > pivot) {
                j = j - 1;
            }
            if (i <= j) {
                double t = a[i];
                a[i] = a[j];
                a[j] = t;
                i = i + 1;
                j = j - 1;
            }
        }

        // Recursion for quicksort
        if (lo < j) {
            quicksort(a, lo, j);
        }
        lo = i;
        j = hi;
    }
}

#define N 10000000

double data[N];

int main()
{
    double t0, t1;

    for (int k=0; k<N; ++k)
        data[k] = random();

    t0 = clock_now();
    quicksort(data, 0, N-1);
    t1 = clock_now();

    printf("size = %d, msec = %.1f\n", N, (t1 - t0) * 1000);
    return 0;
}
