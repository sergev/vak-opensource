#include <iostream>
#include "parser.hh"

namespace Demo {

//
// Invoke the parser.
//
void Parser::parse()
{
    // Clear counters.
    chars = 0;
    words = 0;
    lines = 0;
    uppercase = 0;
    lowercase = 0;

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
    std::cout << "Lines: " << lines << std::endl;
    std::cout << "Words: " << words << std::endl;
    std::cout << "Characters: " << chars << std::endl;
    std::cout << "Uppercase: " << uppercase << std::endl;
    std::cout << "Lowercase: " << lowercase << std::endl;
}

//
// Read next token from input stream.
// Return a token type, as defined in grammar.yy.
// For words, store the string into `lval'.
// For newlines, update the `loc' location.
//
int Parser::get_next_token(Grammar::semantic_type *lval, location *loc)
{
    char c0;
    std::cin.get(c0);
    if (std::cin.eof()) {
        return Grammar::token::END;
    }
    if (c0 == '\n') {
        loc->lines();
        return Grammar::token::NEWLINE;
    }
    if (!isalpha(c0)) {
        return Grammar::token::CHAR;
    }

    // Check next symbol.
    char c;
    std::cin.get(c);
    if (std::cin.eof() || !isalpha(c)) {
        if (!std::cin.eof())
            std::cin.unget();

        if (isupper(c0)) {
            return Grammar::token::UPPER;
        } else {
            return Grammar::token::LOWER;
        }
    }

    // Scan a word.
    std::string text;
    text += c0;
    for (;;) {
        text += c;
        std::cin.get(c);
        if (!isalpha(c)) {
            std::cin.unget();
            break;
        }
    }
    lval->build<std::string>(text);
    return Grammar::token::WORD;
}

} // namespace Demo
