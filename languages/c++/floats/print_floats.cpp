//
// Print all especial float values.
// Use 'running one' and 'running zero' bitmasks for mantissa and exponent.
//
#include <iostream>
#include <climits>
#include <bitset>

const unsigned exponent_tab[] = {
    0,
    0b00000001,
//  0b00000010,
//  0b00000100,
//  0b00001000,
//  0b00010000,
//  0b00100000,
//  0b01000000,
    0b01111111,
    0b10000000,
//  0b10111111,
//  0b11011111,
//  0b11101111,
//  0b11110111,
//  0b11111011,
//  0b11111101,
    0b11111110,
    0b11111111,
    UINT_MAX,
};

const unsigned mantissa_tab[] = {
    0,
    0b00000000000000000000001,
//  0b00000000000000000000010,
//  0b00000000000000000000100,
//  0b00000000000000000001000,
//  0b00000000000000000010000,
//  0b00000000000000000100000,
//  0b00000000000000001000000,
//  0b00000000000000010000000,
//  0b00000000000000100000000,
//  0b00000000000001000000000,
//  0b00000000000010000000000,
//  0b00000000000100000000000,
//  0b00000000001000000000000,
//  0b00000000010000000000000,
//  0b00000000100000000000000,
//  0b00000001000000000000000,
//  0b00000010000000000000000,
//  0b00000100000000000000000,
//  0b00001000000000000000000,
//  0b00010000000000000000000,
//  0b00100000000000000000000,
//  0b01000000000000000000000,
    0b01111111111111111111111,
    0b10000000000000000000000,
//  0b10111111111111111111111,
//  0b11011111111111111111111,
//  0b11101111111111111111111,
//  0b11110111111111111111111,
//  0b11111011111111111111111,
//  0b11111101111111111111111,
//  0b11111110111111111111111,
//  0b11111111011111111111111,
//  0b11111111101111111111111,
//  0b11111111110111111111111,
//  0b11111111111011111111111,
//  0b11111111111101111111111,
//  0b11111111111110111111111,
//  0b11111111111111011111111,
//  0b11111111111111101111111,
//  0b11111111111111110111111,
//  0b11111111111111111011111,
//  0b11111111111111111101111,
//  0b11111111111111111110111,
//  0b11111111111111111111011,
//  0b11111111111111111111101,
    0b11111111111111111111110,
    0b11111111111111111111111,
    UINT_MAX,
};

//
// Single-precision floating-point format
//
// 31 30    23 22                    0
// --|--------|-----------------------
//  0 01111100 01000000000000000000000
//  |     |               |
// sign exponent       mantissa
//       8 bits         23 bits
//
float make_float(unsigned sign, unsigned exponent, unsigned mantissa)
{
    union {
        uint32_t u;
        float f;
    } val;

    val.u = (sign & 1) << 31 |
            (exponent & 0xff) << 23 |
            (mantissa & 0x7fffff);
    return val.f;
}

int main()
{
    unsigned sign, exponent, mantissa;

    std::cout.setf(std::ios::showpoint);
    std::cout.precision(9);
    for (sign=0; sign<2; sign++) {
        for (int e=0; (exponent = exponent_tab[e]) != UINT_MAX; e++) {
            for (int m=0; (mantissa = mantissa_tab[m]) != UINT_MAX; m++) {
                float f = make_float(sign, exponent, mantissa);

                std::cout << std::bitset<1>(sign) << ' '
                          << std::bitset<8>(exponent) << ' '
                          << std::bitset<23>(mantissa) << " = "
                          << f << std::endl;
            }
        }
    }
}
