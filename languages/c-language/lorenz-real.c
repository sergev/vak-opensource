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
    const double H = 0.01;
    const double A = 10.0;
    const double B = 28.0;
    const double C = 8.0 / 3.0;

    double x = 0.1;
    double y = 0;
    double z = 0;

    for (int i = 0; i < N; i++) {
        double x_next = x + H * A * (y - x);
        double y_next = y + H * (x * (B - z) - y);
        double z_next = z + H * (x * y - C * z);
        x = x_next;
        y = y_next;
        z = z_next;
        printf("%d %g %g %g\n", i, x, y, z);
    }
    printf("%g %g %g\n", x, y, z);
}
