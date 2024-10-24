//
// Find the minimum natural number that increases N times after
// moving its last digit to the beginning of the number.
//
#include <iostream>
#include <gmpxx.h>

using bigint = mpz_class; // Integer type with unlimited precision

int main()
{
    for (int n = 2; n < 10; n++) {
        bigint ten_pow_k = 1;
        for (int k = 1; k < 100; k++) {
            ten_pow_k *= 10;
            bigint x = n * (ten_pow_k - n);
            if (x % (10*n - 1) == 0) {
                bigint v = 10 * x / (10*n - 1) + n;
                std::cout << "For " << n << " solution = " << v << '\n';
                break;
            }
        }
    }
}
