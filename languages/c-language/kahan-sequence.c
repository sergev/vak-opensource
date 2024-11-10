//
// A sequence that seems to converge to a wrong limit.
//
// https://perso.ens-lyon.fr/jean-michel.muller/chapitre1.pdf
//
#include <stdio.h>

int main()
{
    double u = 2.;
    double v = -4.;
    int max = 32;

    printf("n = %d\n", max);
    printf("u0 = %f\n", u);
    printf("u1 = %f\n", v);
    printf("Computation from 3 to n:\n");
    for (int i = 3; i <= max; i++) {
        double w = 111. - 1130./v + 3000./(v*u);
        u = v;
        v = w;
        printf("u%d = %1.17g\n", i, v);
    }
}
