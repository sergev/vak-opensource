//
// Print float 1.0+epsilon values.
//
// The epsilon is defined as the difference between 1.0 and
// the next value representable by the floating-point type.
// Value 1.0+epsilon has only the lowest bit of mantissa set to 1.
//
#include <iostream>
#include <climits>
#include <bitset>

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
union Float32 {
    float float32;
    uint32_t uns32;
    struct {
        unsigned mantissa : 23;
        unsigned exponent : 8; // offset 0x7f means '0'
        unsigned sign : 1;
    } field;
};

void print_float(float f)
{
    auto save_flags = std::cout.flags();
    Float32 u;

    u.float32 = f;
    std::cout << std::bitset<1>(u.field.sign) << ' '
              << std::bitset<8>(u.field.exponent) << ' '
              << std::bitset<23>(u.field.mantissa) << " = "
              << f << " = " << std::hexfloat << f << std::endl;

    std::cout.flags(save_flags);
}

int main()
{
    Float32 u;
    u.field.sign = 0;
    u.field.mantissa = 0b00000000000000000000001;

    std::cout.setf(std::ios::showpoint);
    std::cout.precision(9);

    for (u.field.exponent = 1; u.field.exponent < 0xff; u.field.exponent++) {
        print_float(u.float32);
    }

    // For comparison:
    //print_float(1.0 + std::numeric_limits<float>::epsilon());
}
