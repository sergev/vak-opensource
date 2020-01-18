%skeleton "lalr1.cc"
%require  "3.2"
%debug
%defines
%define api.namespace { Demo }
%define api.parser.class { Grammar }
%define api.value.type variant
%define parse.assert
%locations

// Add parameter to the Grammar() constructor.
%code requires {
    namespace Demo {
        class Parser;
    }
}
%parse-param { Demo::Parser &parser }

// Use get_next_token() for reading the input stream.
%code {
    #include "parser.hh"
    #undef yylex
    #define yylex parser.get_next_token
}

// Define tokens.
%token               END    0     "end of file"
%token               UPPER
%token               LOWER
%token <std::string> WORD
%token               NEWLINE
%token               CHAR

%%

list_option : END | list END;

list
    : item
    | list item
    ;

item
    : UPPER {
                // Single character, upper case.
                parser.uppercase++;
                parser.chars++;
                parser.words++;
            }
    | LOWER {
                // Single character, lower case.
                parser.lowercase++;
                parser.chars++;
                parser.words++;
            }
    | WORD {
                // Word, a sequence of characters.
                parser.words++;
                parser.chars += $1.length();
                for (const char &c : $1) {
                    if (isupper(c)) {
                        parser.uppercase++;
                    } else {
                        parser.lowercase++;
                    }
                }
            }
    | NEWLINE {
                // Line delimiter.
                parser.lines++;
                parser.chars++;
            }
    | CHAR {
                // Other symbol.
                parser.chars++;
            }
    ;

%%

void Demo::Grammar::error(const location_type &loc, const std::string &message)
{
    std::cerr << "Error: " << message << " at " << loc << "\n";
}
