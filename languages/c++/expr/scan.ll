/*
 * An example of using the flex C++ scanner class.
 * Based on example from Flex sources:
 * https://github.com/westes/flex/blob/master/examples/testxxLexer.l
 */
%option C++ noyywrap yylineno
%option yyclass="Scanner"
%option prefix="Scanner_"

%{
    #include "parser.hh"

    using namespace Demo;

    // Generate the following scan routine.
    #undef YY_DECL
    #define YY_DECL Grammar::token::yytokentype Scanner::get_next_token(\
        Grammar::semantic_type *const lval, \
        Grammar::location_type *loc)

    // Special action on end of file.
    #define yyterminate() return Grammar::token::END;

    #define YY_USER_ACTION loc->step(); loc->columns(yyleng);
%}

blank   [ \t\r]
%%

[[:digit:]]+ {
    lval->build<int>(std::stoi(yytext));
    return Grammar::token::INT;
}

"+" return Grammar::token::PLUS;
"-" return Grammar::token::MINUS;
"*" return Grammar::token::MUL;
"/" return Grammar::token::DIV;
"%" return Grammar::token::REM;
"(" return Grammar::token::LPAR;
")" return Grammar::token::RPAR;

{blank}+ {
    // Skip spaces.
    loc->step();
}

\n+ {
    // Skip newlines.
    loc->lines(yyleng);
    loc->step();
}

. {
    // Invalid character.
    return Grammar::token::ERROR;
}

%%
