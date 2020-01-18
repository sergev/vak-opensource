%skeleton "lalr1.cc"
%require  "3.2"
%debug
%defines
%define api.namespace { Demo }
%define api.parser.class { Grammar }
%define api.value.type variant
%define parse.assert
%locations

%code {
    #include "demo.hh"
    #undef yylex
    #define yylex get_next_token
}

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
    : UPPER   { Demo::add_upper(); }
    | LOWER   { Demo::add_lower(); }
    | WORD    { Demo::add_word($1); }
    | NEWLINE { Demo::add_newline(); }
    | CHAR    { Demo::add_char(); }
    ;

%%

void Demo::Grammar::error(const location_type &loc, const std::string &message)
{
    std::cerr << "Error: " << message << " at " << loc << "\n";
}
