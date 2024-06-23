#include <string>
#include <memory>
#include <variant>
#include <list>
#include <iostream>

//
// Define type List, each element of which can be integer, string or list.
//
using Integer = long long;
using String  = std::string;
class List;
using Term    = std::variant<Integer, String, List>;

class List : public std::list<Term> {};

std::ostream& operator<<(std::ostream& out, const List &list);

//
// Print a term.
//
std::ostream& operator<<(std::ostream& out, const Term &term)
{
    if (term.index() == std::variant_npos) {
        out << "null";
    } else {
        std::visit([&out](const auto &arg) { out << arg; }, term);
    }
    return out;
}

//
// Print a list, recursively.
//
std::ostream& operator<<(std::ostream& out, const List &list)
{
    out << '{';
    for (const auto& element : list) {
        out << ' ' << element;
    }
    out << " }";
    return out;
}

int main()
{
    std::cout << "sizeof Integer = " << sizeof(Integer) << '\n';
    std::cout << "sizeof String  = " << sizeof(String) << '\n';
    std::cout << "sizeof Term    = " << sizeof(Term) << '\n';
    std::cout << "sizeof List    = " << sizeof(List) << '\n';

    List a;
    a.push_back(1);
    a.push_back("foo");
    a.push_back(a);
    std::cout << "sizeof a       = " << sizeof(a) << '\n';
    std::cout << "a.size()       = " << a.size() << '\n';
    std::cout << "a              = " << a << '\n';
}
