#include <iostream>
#include <map>

int main()
{
    // Convert instruction enum into name.
    static std::map<int, std::string> tab{
        { 0, "nop" },
        { 1, "define" },
    };

    std::cout << "Try map:" << std::endl;
    std::cout << "tab[2] =" << std::endl;
    std::cout << tab[2] << std::endl;
    //std::cout << tab.at(2) << std::endl;
    std::cout << "Done." << std::endl;
    return 0;
}
