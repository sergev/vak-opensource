#ifndef TOKEN_H
#define TOKEN_H

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

#include <string>

#define T_UNKNOWN		0

#define T_AND			1
#define T_CALL			2
#define T_CASE			3
#define T_DO			4
#define T_ELSE			5
#define T_END			6
#define T_ESAC			7
#define T_EXIT			8
#define T_EXTERN		9
#define T_FI			10
#define T_FOR			11
#define T_FROM			12
#define T_FUN			13
#define T_IF			14
#define T_INPUT			15
#define T_INTERN		16
#define T_NLF			17
#define T_NOT			18
#define T_OD			19
#define T_OR			20
#define T_OUTPUT		21
#define T_PROC			22
#define T_REPEAT		23
#define T_RETURN		24
#define T_STEP			25
#define T_TEXT			26
#define T_THEN			27
#define T_TO			28
#define T_UNTIL			29	
#define T_WHEN			30
#define T_WHILE			31

#define T_LPAREN		32
#define T_RPAREN		33
#define T_LBRACKET		34
#define T_RBRACKET		35

#define T_PLUS			36
#define T_MINUS			37
#define T_MULTIPLY		38
#define T_EXPONENT		39
#define T_DIVIDE		40
#define T_INTDIVIDE		41
#define T_REMAINDER		42

#define T_GREATEQ		43
#define T_LESSEQ		44
#define T_GREAT			45
#define T_LESS			46

#define T_EQUAL			47
#define T_SLASHEQ		48

#define T_COLON			49
#define T_ASSIGN		50

#define T_HASH			51

#define T_YES			52
#define T_NO			53

#define T_LARROW		54
#define T_RARROW		55

#define T_COMMA			56
#define T_SEMICOLON		57

#define T_EQGREAT		58

#define T_STRING		59
#define T_IDENTIFIER	60

#define T_UNSIGNEDINT	61
#define T_UNSIGNEDREAL	62

#define T_EOL			63
#define T_EOF			83

#define T_EMPTY			65

class Token
{

	public:

		/*** Constructor ***/
		Token();

		/*** Constructor ***/
		Token(unsigned char pType, std::string pLexeme);

		/*** Set the current token's type ***/
		void setType(unsigned char pType);

		/*** Get the current token's type ***/
		unsigned char getType();

		/*** Set the current token's lexeme ***/
		void setLexeme(std::string pLexeme);

		/*** Get the current token's lexeme ***/
		std::string getLexeme();

		/*** Set the current token's line number ***/
		void setLineNumber(unsigned long pLineNum);

		/*** Get the current token's line number ***/
		unsigned long getLineNumber();

		/*** Set the current token's column number ***/
		void setColumnNumber(unsigned long pColNum);

		/*** Get the current token's column number ***/
		unsigned long getColumnNumber();

		/*** Destructor ***/
		~Token();

	private:

		/*** The current token's type ***/
		unsigned char type;

		/*** The current token's lexeme ***/
		std::string lexeme;

		/*** The current token's line number ***/
		unsigned long lineNum;

		/*** The current token's column number ***/
		unsigned long colNum;

};

#endif
