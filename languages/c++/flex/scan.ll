/*
 * An example of using the flex C++ scanner class.
 * Based on example from Flex sources:
 * https://github.com/westes/flex/blob/master/examples/testxxLexer.l
 */
%option C++ noyywrap yylineno
%option yyclass="Scanner"
%option prefix="Scanner_"

%{
#include "scanner.h"

// Define the scan routine for flex.
#undef YY_DECL
#define YY_DECL Scanner::Token Scanner::get_next_token()

// Special action on end of file.
#define yyterminate() return Scanner::Token::END;
%}

string	\"[^\n"]+\"

ws	[ \t]+

alpha	[A-Za-z]
dig	[0-9]
name	({alpha}|{dig}|\$)({alpha}|{dig}|\_|\.|\-|\/|\$)*
num1	[-+]?{dig}+\.?([eE][-+]?{dig}+)?
num2	[-+]?{dig}*\.{dig}+([eE][-+]?{dig}+)?
number	{num1}|{num2}

%%

\n              // Skip newlines.

{ws}            // Skip blanks and tabs.

"//".*          // Skip one-line comments.

"/*"        {
                // Skip C-style comments.
		int c;

		while ((c = yyinput()) != 0) {
		    if (c == '*') {
                        c = yyinput();
                        if (c == '/')
                            break;
                        unput(c);
		    }
		}
            }

{number}	return Token::NUMBER;

{name}		return Token::NAME;

{string}	return Token::STRING;

%%
