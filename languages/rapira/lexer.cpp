// ReRap Version 0.9
// Copyright 2011 Matthew Mikolay.
//
// This file is part of ReRap.
//
// ReRap is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ReRap is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ReRap.  If not, see <http://www.gnu.org/licenses/>.

#include "lexer.h"

/*** Constructor ***/
Lexer::Lexer(char* filename)
{
	// Open the input file
	file.open(filename, std::ifstream::in);
	lineNum = 1;
	colNum = 0;
	oldLine = 1;
	oldCol = 0;
	comment = false;
}

/*** Determine if the file has been opened ***/
bool Lexer::isOpen()
{
	return file.is_open();
}

/*** Determine if the lexer has more tokens ***/
bool Lexer::hasMoreTokens()
{
	while(hasChars())
	{
		char next = peekChar();

		if(next == '\n' || !isWhitespace(next))
			return true;
		getChar();
	}

	return false;
}

/*** Get the next token ***/
Token Lexer::getNextToken()
{
	// Create a token to return
	Token nextToken;
	nextToken.setType(T_UNKNOWN);

	// Set lastpos to the current position
	lastpos = file.tellg();
	oldLine = lineNum;
	oldCol = colNum;

	// Run through finite state machine checks
	checkEndOfLine(nextToken);

	checkString(nextToken);

	checkKeyword(nextToken, "and", T_AND);
	checkKeyword(nextToken, "call", T_CALL);
	checkKeyword(nextToken, "case", T_CASE);
	checkKeyword(nextToken, "do", T_DO);
	checkKeyword(nextToken, "else", T_ELSE);
	checkKeyword(nextToken, "end", T_END);
	checkKeyword(nextToken, "esac", T_ESAC);
	checkKeyword(nextToken, "exit", T_EXIT);
	checkKeyword(nextToken, "extern", T_EXTERN);
	checkKeyword(nextToken, "fi", T_FI);
	checkKeyword(nextToken, "for", T_FOR);
	checkKeyword(nextToken, "from", T_FROM);
	checkKeyword(nextToken, "fun", T_FUN);
	checkKeyword(nextToken, "if", T_IF);
	checkKeyword(nextToken, "input", T_INPUT);
	checkKeyword(nextToken, "intern", T_INTERN);
	checkKeyword(nextToken, "nlf", T_NLF);
	checkKeyword(nextToken, "not", T_NOT);
	checkKeyword(nextToken, "od", T_OD);
	checkKeyword(nextToken, "or", T_OR);
	checkKeyword(nextToken, "output", T_OUTPUT);
	checkKeyword(nextToken, "proc", T_PROC);
	checkKeyword(nextToken, "repeat", T_REPEAT);
	checkKeyword(nextToken, "return", T_RETURN);
	checkKeyword(nextToken, "step", T_STEP);
	checkKeyword(nextToken, "text", T_TEXT);
	checkKeyword(nextToken, "then", T_THEN);
	checkKeyword(nextToken, "to", T_TO);
	checkKeyword(nextToken, "until", T_UNTIL);
	checkKeyword(nextToken, "when", T_WHEN);
	checkKeyword(nextToken, "while", T_WHILE);

	checkKeyword(nextToken, "yes", T_YES);
	checkKeyword(nextToken, "no", T_NO);
	checkKeyword(nextToken, "empty", T_EMPTY);

	checkUnsignedReal(nextToken);
	checkUnsignedInteger(nextToken);
	checkIdentifier(nextToken);

	checkSymbol(nextToken, "(", T_LPAREN);
	checkSymbol(nextToken, ")", T_RPAREN);
	checkSymbol(nextToken, "[", T_LBRACKET);
	checkSymbol(nextToken, "]", T_RBRACKET);

	checkSymbol(nextToken, "<*", T_LARROW);
	checkSymbol(nextToken, "*>", T_RARROW);

	checkSymbol(nextToken, "+", T_PLUS);
	checkSymbol(nextToken, "-", T_MINUS);
	checkSymbol(nextToken, "**", T_EXPONENT);
	checkSymbol(nextToken, "*", T_MULTIPLY);

	checkSymbol(nextToken, "/=", T_SLASHEQ);
	checkSymbol(nextToken, "//", T_INTDIVIDE);
	checkSymbol(nextToken, "/%", T_REMAINDER);
	checkSymbol(nextToken, "/", T_DIVIDE);

	checkSymbol(nextToken, ":=", T_ASSIGN);
	checkSymbol(nextToken, ":", T_COLON);
	checkSymbol(nextToken, "#", T_HASH);
	checkSymbol(nextToken, ",", T_COMMA);
	checkSymbol(nextToken, ";", T_SEMICOLON);

	checkSymbol(nextToken, "=>", T_EQGREAT);
	checkSymbol(nextToken, ">=", T_GREATEQ);
	checkSymbol(nextToken, "<=", T_LESSEQ);
	checkSymbol(nextToken, ">", T_GREAT);
	checkSymbol(nextToken, "<", T_LESS);
	checkSymbol(nextToken, "=", T_EQUAL);

	if(nextToken.getType() == T_UNKNOWN)
	{
		// Collect the next few characters
		std::string errMessage = "\"";
		for(unsigned char i = 0; hasChars() && (i < 10); i++)
			errMessage.push_back(getChar());
		errMessage.append("...\"");

		throw LexerSyntaxException(oldLine, oldCol, errMessage);
	}

	return nextToken;
}

/*** Destructor ***/
Lexer::~Lexer()
{
	// Close the input file
	file.close();
}

/*** Return true if the file has more characters ***/
bool Lexer::hasChars()
{
	return (file.good() && file.peek() != EOF);
}

/*** Peek at the next character ***/
char Lexer::peekChar()
{
	char result = file.peek();

	if(result == '\\')
		comment = true;

	if(comment)
		return ' ';
	
	return result;
}

/*** Get a character ***/
char Lexer::getChar()
{
	char result = file.get();

	if(result == '\\')
		comment = true;

	if(result == '\n')
	{
		// Reset the line and column numbers
		lineNum++;
		colNum = 0;
		comment = false;
	}

	// Increment the column number
	colNum++;

	// If the lexer is currently processing
	// a comment, then return whitespace
	if(comment)
		return ' ';

	return result;
}

/*** Fail a finite state machine token test ***/
void Lexer::fail()
{
	// Reset the position of the get pointer
	file.seekg(lastpos);
	lineNum = oldLine;
	colNum = oldCol;
}

void Lexer::checkEndOfLine(Token &target)
{
	// If the token already has a type, skip the check
	if(target.getType() != T_UNKNOWN)
		return;

	// Create a token to return
	Token result;
	result.setType(T_EOL);
	result.setLineNumber(lineNum);
	result.setLexeme("EOL");

	if(getChar() == '\n')
	{
		target = result;
		return;
	}

	fail();
}

void Lexer::checkString(Token &target)
{
	// If the token already has a type, skip the check
	if(target.getType() != T_UNKNOWN)
		return;

	// Create a token to return
	Token result;
	result.setType(T_STRING);
	result.setLineNumber(lineNum);
	result.setColumnNumber(colNum);

	std::string lexeme = "";

	if(getChar() != '"')
	{
		fail();
		return;
	}

	while(hasChars() && peekChar() != '"')
	{
		char next = getChar();

		if(next == '\n')
		{
			fail();
			return;
		}

		lexeme.push_back(next);
	}

	if(getChar() != '"')
	{
		fail();
		return;
	}

	result.setLexeme(lexeme);
	target = result;
}

void Lexer::checkIdentifier(Token &target)
{
	// If the token already has a type, skip the check
	if(target.getType() != T_UNKNOWN)
		return;

	// Create a token to return
	Token result;
	result.setType(T_IDENTIFIER);
	result.setLineNumber(lineNum);
	result.setColumnNumber(colNum);

	std::string lexeme = "";

	if(!hasChars() || !isAlpha(peekChar()))
	{
		fail();
		return;
	}

	do
	{
		lexeme.push_back(tolower(getChar()));
	}
	while(hasChars() && (isAlpha(peekChar()) || isNumber(peekChar()) || peekChar() == '_'));

	result.setLexeme(lexeme);
	target = result;
}

void Lexer::checkKeyword(Token &target, std::string keyword, unsigned char type)
{
	// If the token already has a type, skip the check
	if(target.getType() != T_UNKNOWN)
		return;

	// Create a token to return
	Token result;
	result.setType(type);
	result.setLexeme(keyword);
	result.setLineNumber(lineNum);
	result.setColumnNumber(colNum);

	for(unsigned int i = 0; i < keyword.length(); i++)
	{
		// Get the next character from the file
		if(!hasChars() || keyword.at(i) != tolower(getChar()))
		{
			fail();
			return;
		}
	}

	// Confirm that the next character is whitespace or a special character
	if(hasChars() && ((!isWhitespace(peekChar()) && !isSpecial(peekChar())) || peekChar() == '_'))
	{
		fail();
		return;
	}

	target = result;
}

void Lexer::checkSymbol(Token &target, std::string keyword, unsigned char type)
{
	// If the token already has a type, skip the check
	if(target.getType() != T_UNKNOWN)
		return;

	// Create a token to return
	Token result;
	result.setType(type);
	result.setLexeme(keyword);
	result.setLineNumber(lineNum);
	result.setColumnNumber(colNum);

	for(unsigned int i = 0; i < keyword.length(); i++)
	{
		// Get the next character from the file
		if(!hasChars() || keyword.at(i) != getChar())
		{
			fail();
			return;
		}
	}

	target = result;
}

void Lexer::checkUnsignedInteger(Token &target)
{
	// If the token already has a type, skip the check
	if(target.getType() != T_UNKNOWN)
		return;

	// Create a token to return
	Token result;
	result.setType(T_UNSIGNEDINT);
	result.setLineNumber(lineNum);

	std::string lexeme = "";

	if(!hasChars() || !isNumber(peekChar()))
	{
		fail();
		return;
	}

	do
	{
		lexeme.push_back(getChar());
	}
	while(hasChars() && isNumber(peekChar()));

	result.setLexeme(lexeme);
	target = result;
}

void Lexer::checkUnsignedReal(Token &target)
{

	// If the token already has a type, skip the check
	if(target.getType() != T_UNKNOWN)
		return;

	// Create a token to return
	Token result;
	result.setType(T_UNSIGNEDREAL);
	result.setLineNumber(lineNum);

	std::string lexeme = "";

	unsigned char state = 0;
	while(hasChars())
	{
		switch(state)
		{
			case 0:
				if(isNumber(peekChar()))
				{
					lexeme.push_back(getChar());
					state = 1;
				}
				else
				{
					fail();
					return;
				}
				break;
			case 1:
				if(isNumber(peekChar()))
					lexeme.push_back(getChar());
				else if(peekChar() == '.')
				{
					lexeme.push_back(getChar());
					state = 2;
				}
				else
				{
					fail();
					return;
				}
				break;
			case 2:
				if(isNumber(peekChar()))
				{
					lexeme.push_back(getChar());
					state = 3;
				}
				else
				{
					fail();
					return;
				}
				break;
			case 3:
				if(isNumber(peekChar()))
					lexeme.push_back(getChar());
				else if(peekChar() == 'e' || peekChar() == 'E')
				{
					lexeme.push_back(getChar());
					state = 4;
				}
				else
				{
					result.setLexeme(lexeme);
					target = result;
					return;
				}
				break;
			case 4:
				if(peekChar() == '+' || peekChar() == '-')
				{
					lexeme.push_back(getChar());
					state = 5;
				}
				else if(isNumber(peekChar()))
				{
					lexeme.push_back(getChar());
					state = 6;
				}
				else
				{
					fail();
					return;
				}
				break;
			case 5:
				if(isNumber(peekChar()))
				{
					lexeme.push_back(getChar());
					state = 6;
				}
			case 6:
				if(isNumber(peekChar()))
					lexeme.push_back(getChar());
				else
				{
					result.setLexeme(lexeme);
					target = result;
					return;
				}
				break;
				
		}
	}

	fail();
}

bool Lexer::isWhitespace(char input)
{
	return (input == ' ' || input == '\t' || input == '\n');
}

bool Lexer::isSpecial(char input)
{
	return (input == '.' || input == ',' || input == ':' || input == ';' || input == '(' || input == ')' || input == '[' || input == ']' || input == '<' || input == '>' || input == '_' || input == '+' || input == '-' || input == '*' || input == '/' || input == '%' || input == '#' || input == '=' || input == '"');
}

bool Lexer::isAlpha(char input)
{
	return ((input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z'));
}

bool Lexer::isNumber(char input)
{
	return (input >= '0' && input <= '9');
}
