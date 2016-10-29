#include <stdio.h>
#include <fenv.h>

//volatile long double one = 1.0;
#define one 1.0
volatile long double q;

void test(int round, const char *title)
{
    long double r;
    double n;
    int count = 0;
    int less = 0;
    int greater = 0;

    printf ("--- %s ---\n", title);
    fesetround(round);

    for (n=3; n<=1000000; n+=2) {
        q = one / n;
        r = q * n;

        count++;
        if (r == one)
            continue;

        if (r < one)
            less++;
        else
            greater++;
    }
    printf("less %.1f%%, greater %.1f%%\n", less * 100.0 / count, greater * 100.0 / count);

}

int main ()
{
    test(FE_TONEAREST, "Round to nearest");

    test(FE_UPWARD, "Round toward +infinity");

    test(FE_DOWNWARD, "Round toward -infinity");

    test(FE_TOWARDZERO, "Round toward zero");
    return 0;
}
