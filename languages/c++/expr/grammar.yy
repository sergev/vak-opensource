%{
    #include <iostream>
    #include <string>
    #include <map>

    static std::map<std::string, int> vars;

    inline void yyerror(const char *s) { std::cout << s << std::endl; }

    extern int yylex();
%}

%union {
    int i;
    std::string *s;
}

%token BATATA
%token<i> INT
%token<s> VAR
%type<i> expr

%right '='
%left '+' '-'
%left '*' '/' '%'
%right BATATA

%%

list: stmt
    | list stmt
    ;

stmt: expr ','
    | expr ':'                  { std::cout << $1 << std::endl; }
    ;

expr: INT                       { $$ = $1; }
    | VAR                       { $$ = vars[*$1];      delete $1; }
    | VAR '=' expr              { $$ = vars[*$1] = $3; delete $1; }
    | expr '+' expr             { $$ = $1 + $3; }
    | expr '-' expr             { $$ = $1 - $3; }
    | expr '*' expr             { $$ = $1 * $3; }
    | expr '/' expr             { $$ = $1 / $3; }
    | expr '%' expr             { $$ = $1 % $3; }
    | '+' expr %prec BATATA     { $$ =  $2; }
    | '-' expr %prec BATATA     { $$ = -$2; }
    | '(' expr ')'              { $$ =  $2; }
    ;

%%

int main()
{
    yyparse();
    return 0;
}
