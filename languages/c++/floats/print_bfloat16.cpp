//
// Print all especial bfloat16 values.
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
    0b0000001,
//  0b0000010,
//  0b0000100,
//  0b0001000,
//  0b0010000,
//  0b0100000,
    0b0111111,
    0b1000000,
//  0b1011111,
//  0b1101111,
//  0b1110111,
//  0b1111011,
//  0b1111101,
    0b1111110,
    0b1111111,
    UINT_MAX,
};

//
// Bfloat16 floating-point format
//
// 15 14     7 6    0
// --|--------|-------
//  0 01111100 0100000
//  |     |        |
// sign exponent mantissa
//       8 bits   7 bits
//
float make_bfloat16(unsigned sign, unsigned exponent, unsigned mantissa)
{
    union {
        uint32_t u;
        float f;
    } val;

    val.u = (sign & 1) << 31 |
            (exponent & 0xff) << 23 |
            (mantissa & 0x7f) << 16;
    return val.f;
}

int main()
{
    unsigned sign, exponent, mantissa;

    std::cout.setf(std::ios::showpoint);
    std::cout.precision(8);
    for (sign=0; sign<2; sign++) {
        for (int e=0; (exponent = exponent_tab[e]) != UINT_MAX; e++) {
            for (int m=0; (mantissa = mantissa_tab[m]) != UINT_MAX; m++) {
                float f = make_bfloat16(sign, exponent, mantissa);

                std::cout << std::bitset<1>(sign) << ' '
                          << std::bitset<8>(exponent) << ' '
                          << std::bitset<7>(mantissa) << " = "
                          << f << std::endl;
            }
        }
    }
}
