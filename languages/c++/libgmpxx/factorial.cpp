#include <iostream>
#include <gmpxx.h>

int main()
{
    auto const a = mpz_class::factorial(10);
    auto const b = mpz_class::factorial(100);
    auto const c = mpz_class::factorial(1000);
    std::cout << "factorial 10 = " << a << '\n';
    std::cout << "factorial 100 = " << b << '\n';
    std::cout << "factorial 1000 = " << c << '\n';
}
