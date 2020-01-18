%option noyywrap noinput nounput
%{
    #include <string>
    #include "grammar.tab.hh"
%}

%%

[_[:alpha:]][_[:alnum:]]*     yylval.s = new std::string(yytext); return VAR;
[[:digit:]]+                  yylval.i = std::stoi(yytext);       return INT;
[-+*/%=^:,]                   return *yytext;
.|\n                          ; /* ignore all the rest */

%%
