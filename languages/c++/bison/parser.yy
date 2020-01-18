%skeleton "lalr1.cc"
%require  "3.2"
%debug
%defines
%define api.namespace { Demo }
%define api.parser.class { Parser }

%code requires {
    namespace Demo {
        class Driver;
        class Scanner;
    }
}

%parse-param { Scanner &scanner }
%parse-param { Driver &driver }

%code {
    #include <iostream>
    #include <cstdlib>
    #include <fstream>

    /* include for all driver functions */
    #include "driver.hh"

    #undef yylex
    #define yylex scanner.get_next_token
}

%define api.value.type variant
%define parse.assert

%token               END    0     "end of file"
%token               UPPER
%token               LOWER
%token <std::string> WORD
%token               NEWLINE
%token               CHAR

%locations

%%

list_option : END | list END;

list
    : item
    | list item
    ;

item
    : UPPER   { driver.add_upper(); }
    | LOWER   { driver.add_lower(); }
    | WORD    { driver.add_word($1); }
    | NEWLINE { driver.add_newline(); }
    | CHAR    { driver.add_char(); }
    ;

%%

void Demo::Parser::error(const location_type &loc, const std::string &message)
{
    std::cerr << "Error: " << message << " at " << loc << "\n";
}
