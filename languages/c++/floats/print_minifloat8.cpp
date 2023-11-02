//
// Print all minifloat8 values.
//
#include <iostream>
#include <climits>
#include <bitset>

//
// Minifloat8 floating-point format
//
//  7 6 5 4 3 2 1 0
// --|-------|------
//  0 0 1 1 0 0 1 0
//  |     |     |
// sign   |   mantissa
//        |    3 bits
//      exponent
//       4 bits
//
float make_minifloat8(unsigned sign, unsigned exponent, unsigned mantissa)
{
    union {
        uint32_t u;
        float f;
    } val;

    if (exponent == 0xf) {
        // Infinity or NaN.
        exponent = 0xff;
    } else if (exponent != 0) {
        // Normal value.
        exponent += 127 - 7;
    } else if (mantissa == 0) {
        // Zero.
        return sign ? -0.0 : +0.0;
    } else {
        // Denormal.
        return mantissa * 0x1p-9 * (sign ? -1 : +1);
    }

    mantissa <<= 20;
    val.u = (sign & 1) << 31 |
            (exponent & 0xff) << 23 |
            (mantissa & 0x7fffff);
    return val.f;
}

int main()
{
    std::cout.setf(std::ios::showpoint);
    std::cout.precision(4);
    for (unsigned sign = 0; sign < 2; sign++) {
        for (unsigned exponent = 0; exponent < 16; exponent++) {
            for (unsigned mantissa = 0; mantissa < 8; mantissa++) {
                float f = make_minifloat8(sign, exponent, mantissa);

                std::cout << std::bitset<1>(sign) << ' '
                          << std::bitset<4>(exponent) << ' '
                          << std::bitset<3>(mantissa) << " = "
                          //<< std::hexfloat << f << " = "
                          << std::defaultfloat << f << std::endl;
            }
        }
    }
}
