#include <iostream>
#include "demo.hh"

namespace {
    // Local data.
    unsigned chars     = 0;
    unsigned words     = 0;
    unsigned lines     = 0;
    unsigned uppercase = 0;
    unsigned lowercase = 0;
};

int main()
{
    Demo::Grammar grammar;

    if (grammar.parse() != 0) {
        std::cerr << "Parse failed!\n";
    } else {
        std::cout << "Lines: " << lines << std::endl;
        std::cout << "Words: " << words << std::endl;
        std::cout << "Characters: " << chars << std::endl;
        std::cout << "Uppercase: " << uppercase << std::endl;
        std::cout << "Lowercase: " << lowercase << std::endl;
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
        //std::cout << __func__ << " return END" << std::endl;
        return Demo::Grammar::token::END;
    }
    if (c0 == '\n') {
        loc->lines();
        //std::cout << __func__ << " return NEWLINE" << std::endl;
        return Demo::Grammar::token::NEWLINE;
    }
    if (!is_alpha(c0)) {
        //std::cout << __func__ << " return CHAR " << c0 << std::endl;
        return Demo::Grammar::token::CHAR;
    }

    // Check next symbol.
    char c;
    std::cin.get(c);
    if (std::cin.eof() || !is_alpha(c)) {
        if (!std::cin.eof())
            std::cin.unget();

        if (is_upper(c0)) {
            //std::cout << __func__ << " return UPPER " << c0 << std::endl;
            return Demo::Grammar::token::UPPER;
        } else {
            //std::cout << __func__ << " return LOWER " << c0 << std::endl;
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
    //std::cout << __func__ << " return WORD " << text << std::endl;
    return Demo::Grammar::token::WORD;
}

void Demo::add_upper()
{
    uppercase++;
    chars++;
    words++;
}

void Demo::add_lower()
{
    lowercase++;
    chars++;
    words++;
}

void Demo::add_word(const std::string &word)
{
    words++;
    chars += word.length();
    for (const char &c : word) {
        if (islower(c)) {
            lowercase++;
        } else if (isupper(c)) {
            uppercase++;
        }
    }
}

void Demo::add_newline()
{
    lines++;
    chars++;
}

void Demo::add_char()
{
    chars++;
}
