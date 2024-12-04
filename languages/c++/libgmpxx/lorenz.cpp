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
#include <iostream>
#include <gmpxx.h>

using rational = mpq_class; // Rational numbers with unlimited precision

int main()
{
    const int N{ 30 };
    const rational H{ 1, 100 };
    const rational A{ 10 };
    const rational B{ 28 };
    const rational C{ 8, 3 };

    rational x{ 1, 10 };
    rational y{ 0 };
    rational z{ 0 };

    for (int i = 0; i < N; i++) {
        rational x_next = x + H * A * (y - x);
        rational y_next = y + H * (x * (B - z) - y);
        rational z_next = z + H * (x * y - C * z);
        x = x_next;
        y = y_next;
        z = z_next;
        std::cout << i << ' ' << x.get_d() << ' ' << y.get_d() << ' ' << z.get_d() << '\n';
    }
}
