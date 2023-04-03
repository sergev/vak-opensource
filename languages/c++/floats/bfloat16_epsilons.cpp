//
// Print all especial bfloat16 values.
// Use 'running one' and 'running zero' bitmasks for mantissa and exponent.
//
#include <iostream>
#include <climits>
#include <bitset>

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
union BFloat16 {
    float float32;
    uint32_t uns32;
    struct {
        unsigned zero : 16;
        unsigned mantissa : 7;
        unsigned exponent : 8; // offset 0x7f means '0'
        unsigned sign : 1;
    } field;
};

void print_bfloat16(float f)
{
    auto save_flags = std::cout.flags();
    BFloat16 u;

    u.float32 = f;
    std::cout << std::bitset<1>(u.field.sign) << ' '
              << std::bitset<8>(u.field.exponent) << ' '
              << std::bitset<7>(u.field.mantissa) << " = "
              << f << " = " << std::hexfloat << f << std::endl;

    std::cout.flags(save_flags);
}

int main()
{
    BFloat16 u{};
    u.field.mantissa = 0b0000001;

    std::cout.setf(std::ios::showpoint);
    std::cout.precision(8);

    for (u.field.exponent = 1; u.field.exponent < 0xff; u.field.exponent++) {
        print_bfloat16(u.float32);
    }
}
