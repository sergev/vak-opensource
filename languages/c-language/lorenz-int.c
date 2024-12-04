//
// The lorenz attractor was first studied by Ed N. Lorenz,
// a meteorologist, around 1963. It was derived from a simplified
// model of convection in the earth's atmosphere. It also arises
// naturally in models of lasers and dynamos. The system is most
// commonly expressed as 3 coupled non-linear differential equations.
//
//  dx / dt = a (y - x)
//  dy / dt = x (b - z) - y
//  dz / dt = xy - c z
//
// One commonly used set of constants is a = 10, b = 28, c = 8/3.
//
// The series does not form limit cycles nor does it ever reach
// a steady state. Instead it is an example of deterministic chaos.
// As with other chaotic systems the Lorenz system is sensitive
// to the initial conditions, two initial states no matter how close
// will diverge, usually sooner rather than later.
//
// https://paulbourke.net/fractals/lorenz/
//
#include "stdio.h"

int main()
{
    const int N = 500000000;
    const long long RH = 100;
    const long long A = 10;
    const long long B = 28;
    const long long CN = 8;
    const long long CD = 3;

    long long x = 1000000000 / 10;
    long long y = 0;
    long long z = 0;

    for (int i = 0; i < N; i++) {
        long long x_next = x + A * (y - x) / RH;
        long long y_next = y + (x * (B*1000000000 - z) - y*1000000000) / RH / 1000000000;
        long long z_next = z + (x * y - CN * z * 1000000000 / CD) / RH / 1000000000;
        x = x_next;
        y = y_next;
        z = z_next;
        //printf("%d %lld %lld %lld\n", i, x, y, z);
    }
    printf("%lld %lld %lld\n", x, y, z);
}
