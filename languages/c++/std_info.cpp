//
// How much information about your computer you can get from
// the standard C++ library?
//
#include <iostream>
#include <limits>
//#include <cmath>

auto print(std::float_round_style frs)
{
    switch (frs) {
    case std::round_indeterminate:
        return "Cannot be determined";
    case std::round_toward_zero:
        return "Rounding toward zero";
    case std::round_to_nearest:
        return "Rounding toward nearest";
    case std::round_toward_infinity:
        return "Rounding toward positive infinity";
    case std::round_toward_neg_infinity:
        return "Rounding toward negative infinity";
    default:
        return "Unknown round style";
    }
}

int main()
{
    //
    // Numeric limits.
    //
    std::cout << std::numeric_limits<float>::max_digits10
              << " - Number of decimal digits in float\n";
    std::cout << std::numeric_limits<double>::max_digits10
              << " - Number of decimal digits in double\n";
    std::cout << std::numeric_limits<long double>::max_digits10
              << " - Number of decimal digits in long double\n";

    std::cout << print(std::numeric_limits<double>::round_style)
              << " - Rounding style for double\n";
#if 0
.has_denorm

.denorm_min() - when has_denorm

std::filesystem::current_path() - absolute path of the current working directory

.file_status(filename) - get file type and file permissions

.file_size(filename)

.space(dirname) - info about filesystem: capacity, free, available

.directory_iterator

.recursive_directory_iterator

std::thread::hardware_concurrency() - number of concurrent threads

std::hardware_constructive_interference_size - cache line size to promote true sharing

std::hardware_destructive_interference_size - cache line size to avoid false sharing
#endif
}
