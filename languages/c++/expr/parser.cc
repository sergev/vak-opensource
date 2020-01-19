#include <iostream>
#include "parser.hh"

namespace Demo {

//
// Invoke the parser.
//
void Parser::parse()
{
    result = 0;

    // Parse the input stream.
    Grammar grammar(*this);
    if (grammar.parse() != 0) {
        std::cerr << "Parse failed!\n";
    }
}

//
// Print the results.
//
void Parser::print()
{
    std::cout << "Result = " << result << std::endl;
}
} // namespace Demo
