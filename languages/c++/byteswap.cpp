#include <iostream>
#include <algorithm>

template <typename T>
T byteswap(T value)
{
    auto start = reinterpret_cast<std::byte*>(&value);
    auto end   = start + sizeof(value);

    std::reverse(start, end);
    return value;
}

int main()
{
    std::cout << std::hex << 0x123456789abcULL
              << " -> " << byteswap(0x123456789abcULL)
              << std::endl;
}
