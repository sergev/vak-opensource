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
    #define yylex parser.scanner.get_next_token
}

%token       END    0 "end of file"
%token       ERROR
%token       PLUS
%token       MINUS
%token       MUL
%token       DIV
%token       REM
%token       LPAR
%token       RPAR
%token       BATATA
%token <int> INT

%type <int> expr

%left PLUS MINUS
%left MUL DIV REM
%right BATATA

%%

input: expr                     { parser.result = $1; }
    ;

expr: INT                       { $$ = $1; }
    | expr PLUS expr            { $$ = $1 + $3; }
    | expr MINUS expr           { $$ = $1 - $3; }
    | expr MUL expr             { $$ = $1 * $3; }
    | expr DIV expr             { $$ = $1 / $3; }
    | expr REM expr             { $$ = $1 % $3; }
    | PLUS expr %prec BATATA    { $$ =  $2; }
    | MINUS expr %prec BATATA   { $$ = -$2; }
    | LPAR expr RPAR            { $$ =  $2; }
    ;

%%

void Demo::Grammar::error(const location_type &loc, const std::string &message)
{
    std::cerr << "Error: " << message << " at " << loc << "\n";
}
