#include <iostream>
#include <limits>
#include <cmath>

int main()
{
    const double PI = std::atan(1.0) * 4;

    std::cout.precision(std::numeric_limits<double>::max_digits10);
    std::cout << "Pi = " << PI << std::endl;
    return 0;
}
