#include <iostream>
#include "demo.hh"

unsigned chars     = 0;
unsigned words     = 0;
unsigned lines     = 0;
unsigned uppercase = 0;
unsigned lowercase = 0;

int main()
{
    Demo::Grammar grammar;

    if (grammar.parse() != 0) {
        std::cerr << "Parse failed!\n";
    } else {
        std::cout << "Lines: " << Demo::lines << std::endl;
        std::cout << "Words: " << Demo::words << std::endl;
        std::cout << "Characters: " << Demo::chars << std::endl;
        std::cout << "Uppercase: " << Demo::uppercase << std::endl;
        std::cout << "Lowercase: " << Demo::lowercase << std::endl;
    }
    return 0;
}

static bool is_upper(char c)
{
    return (c >= 'A' && c <= 'Z');
}

static bool is_lower(char c)
{
    return (c >= 'a' && c <= 'z');
}

static bool is_alpha(char c)
{
    return is_upper(c) || is_lower(c);
}

int Demo::get_next_token(Demo::Grammar::semantic_type *lval, Demo::location *loc)
{
    char c0;
    std::cin.get(c0);
    if (std::cin.eof()) {
        return Demo::Grammar::token::END;
    }
    if (c0 == '\n') {
        loc->lines();
        return Demo::Grammar::token::NEWLINE;
    }
    if (!is_alpha(c0)) {
        return Demo::Grammar::token::CHAR;
    }

    // Check next symbol.
    char c;
    std::cin.get(c);
    if (std::cin.eof() || !is_alpha(c)) {
        if (!std::cin.eof())
            std::cin.unget();

        if (is_upper(c0)) {
            return Demo::Grammar::token::UPPER;
        } else {
            return Demo::Grammar::token::LOWER;
        }
    }

    // Scan a word.
    std::string text;
    text += c0;
    for (;;) {
        text += c;
        std::cin.get(c);
        if (!is_alpha(c)) {
            std::cin.unget();
            break;
        }
    }
    lval->build<std::string>(text);
    return Demo::Grammar::token::WORD;
}
