#include <iostream>
#include <gmpxx.h>

int main()
{
    auto const a = mpz_class::fibonacci(10);
    auto const b = mpz_class::fibonacci(100);
    auto const c = mpz_class::fibonacci(1000);
    std::cout << "fibonacci 10 = " << a << '\n';
    std::cout << "fibonacci 100 = " << b << '\n';
    std::cout << "fibonacci 1000 = " << c << '\n';
}
