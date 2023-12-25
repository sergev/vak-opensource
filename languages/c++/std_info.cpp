//
// How much information about your computer you can get from
// the standard C++ library?
//
#include <iostream>
#include <sstream>
#include <limits>
#include <filesystem>
#include <thread>
#include <new>

auto print(std::float_round_style frs)
{
    switch (frs) {
    case std::round_indeterminate:       return "Cannot be determined";
    case std::round_toward_zero:         return "Rounding toward zero";
    case std::round_to_nearest:          return "Rounding toward nearest";
    case std::round_toward_infinity:     return "Rounding toward positive infinity";
    case std::round_toward_neg_infinity: return "Rounding toward negative infinity";
    default:                             return "Unknown round style";
    }
}

auto print(std::filesystem::file_type type)
{
    switch (type) {
    case std::filesystem::file_type::none:      return "None";
    case std::filesystem::file_type::not_found: return "Notexistent";
    case std::filesystem::file_type::regular:   return "Regular file";
    case std::filesystem::file_type::directory: return "Directory";
    case std::filesystem::file_type::symlink:   return "Symlink";
    case std::filesystem::file_type::block:     return "Block device";
    case std::filesystem::file_type::character: return "Character device";
    case std::filesystem::file_type::fifo:      return "Named IPC pipe";
    case std::filesystem::file_type::socket:    return "Named IPC socket";
    case std::filesystem::file_type::unknown:   return "Unknown";
    default:                                    return "Implementation-specific";
    }
}

auto print(std::filesystem::perms p)
{
    std::stringstream out;
    out << ((p & std::filesystem::perms::owner_read)   == std::filesystem::perms::none ? '-' : 'r');
    out << ((p & std::filesystem::perms::owner_write)  == std::filesystem::perms::none ? '-' : 'w');
    out << ((p & std::filesystem::perms::owner_exec)   == std::filesystem::perms::none ? '-' : 'x');
    out << ((p & std::filesystem::perms::group_read)   == std::filesystem::perms::none ? '-' : 'r');
    out << ((p & std::filesystem::perms::group_write)  == std::filesystem::perms::none ? '-' : 'w');
    out << ((p & std::filesystem::perms::group_exec)   == std::filesystem::perms::none ? '-' : 'x');
    out << ((p & std::filesystem::perms::others_read)  == std::filesystem::perms::none ? '-' : 'r');
    out << ((p & std::filesystem::perms::others_write) == std::filesystem::perms::none ? '-' : 'w');
    out << ((p & std::filesystem::perms::others_exec)  == std::filesystem::perms::none ? '-' : 'x');
    return out.str();
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

    std::cout << std::numeric_limits<float>::min()
              << " - Minimal value for float\n";
    std::cout << std::numeric_limits<double>::min()
              << " - Minimal value for double\n";
    std::cout << std::numeric_limits<long double>::min()
              << " - Minimal value for long double\n";

    if (std::numeric_limits<float>::has_denorm) {
        std::cout << "Denorm - Supported for float\n";
        std::cout << std::numeric_limits<float>::denorm_min()
                  << " - Minimal value of denorm for float\n";
    } else {
        std::cout << "No denorm - Unsupported for float\n";
    }

    if (std::numeric_limits<double>::has_denorm) {
        std::cout << "Denorm - Supported for double\n";
        std::cout << std::numeric_limits<double>::denorm_min()
                  << " - Minimal value of denorm for double\n";
    } else {
        std::cout << "No denorm - Unsupported for double\n";
    }

    if (std::numeric_limits<long double>::has_denorm) {
        std::cout << "Denorm - Supported for long double\n";
        std::cout << std::numeric_limits<long double>::denorm_min()
                  << " - Minimal value of denorm for long double\n";
    } else {
        std::cout << "No denorm - Unsupported for long double\n";
    }

    //
    // Filesystem.
    //
    std::cout << std::filesystem::current_path()
              << " - Current working directory\n";
    std::cout << print(std::filesystem::status("/").type())
              << " - Type of root directory\n";
    std::cout << print(std::filesystem::status("/").permissions())
              << " - Permissions of root directory\n";
    std::cout << std::filesystem::file_size("/bin/sh")
              << " - Size of /bin/sh file in bytes\n";

    auto const space = std::filesystem::space("/");
    std::cout << space.capacity
              << " - Capacity of root filesystem, bytes\n";
    std::cout << space.free
              << " - Free space in root filesystem, bytes\n";
    std::cout << space.available
              << " - Available space in root filesystem, bytes\n";

    std::cout << "Files in current directory:\n";
    for (auto const &item : std::filesystem::directory_iterator{"."}) {
        std::cout << item.path() << '\n';
    }

    std::cout << "Files in current directory, recursively:\n";
    for (auto const &item : std::filesystem::recursive_directory_iterator{"."}) {
        std::cout << item.path() << '\n';
    }

    //
    // Threads.
    //
    std::cout << std::thread::hardware_concurrency()
              << " - Number of hardware threads\n";

    //
    // Cache.
    //
#ifdef __cpp_lib_hardware_interference_size
    std::cout << std::hardware_constructive_interference_size
              << " - Ccache line size to promote true sharing\n";
    std::cout << std::hardware_destructive_interference_size
              << " - Ccache line size to avoid false sharing\n";
#endif
}
