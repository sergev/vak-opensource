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
    : UPPER {
                Demo::uppercase++;
                Demo::chars++;
                Demo::words++;
            }
    | LOWER {
                Demo::lowercase++;
                Demo::chars++;
                Demo::words++;
            }
    | WORD {
                Demo::words++;
                Demo::chars += $1.length();
                for (const char &c : $1) {
                    if (isupper(c)) {
                        Demo::uppercase++;
                    } else {
                        Demo::lowercase++;
                    }
                }
            }
    | NEWLINE {
                Demo::lines++;
                Demo::chars++;
            }
    | CHAR {
                Demo::chars++;
            }
    ;

%%

void Demo::Grammar::error(const location_type &loc, const std::string &message)
{
    std::cerr << "Error: " << message << " at " << loc << "\n";
}
