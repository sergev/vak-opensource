#ifndef LEXER_H
#define LEXER_H

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

#include <fstream>
#include "token.h"
#include "exceptions/lexersyntax.h"

class Lexer
{
	public:

		/*** Constructor ***/
		Lexer(char* filename);

		/*** Determine if the file has been opened ***/
		bool isOpen();

		/*** Determine if the lexer has more tokens ***/
		bool hasMoreTokens();

		/*** Get the next token ***/
		Token getNextToken();

		/*** Destructor ***/
		~Lexer();

	private:

		/*** Input file ***/
		std::ifstream file;

		/*** The file's current line number ***/
		unsigned int lineNum;

		/*** The file's current column number ***/
		unsigned int colNum;

		/*** If the lexer is currently processing a comment ***/
		bool comment;

		/*** The last position ***/
		/*** Used to reset position of get pointer ***/
		/*** When fail method is called ***/
		std::streampos lastpos;
		unsigned int oldLine;
		unsigned int oldCol;

		/*** Return true if the file has more characters ***/
		bool hasChars();

		/*** Peek at the next character ***/
		char peekChar();

		/*** Get a character ***/
		char getChar();

		/*** Fail a finite state machine token test ***/
		void fail();

		/*** Finite state machines to check stream data ***/
		void checkEndOfLine(Token &target);
		void checkString(Token &target);
		void checkIdentifier(Token &target);
		void checkKeyword(Token &target, std::string keyword, unsigned char type);
		void checkSymbol(Token &target, std::string keyword, unsigned char type);
		void checkUnsignedInteger(Token &target);
		void checkUnsignedReal(Token &target);

		bool isWhitespace(char input);
		bool isSpecial(char input);
		bool isAlpha(char input);
		bool isNumber(char input);

};

#endif

