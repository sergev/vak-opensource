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

#include "token.h"

/*** Constructor ***/
Token::Token()
{
	setType(T_UNKNOWN);
	setLineNumber(0);
	setColumnNumber(0);
	setLexeme("");
}

/*** Constructor ***/
Token::Token(unsigned char pType, std::string pLexeme)
{
	setType(pType);
	setLineNumber(0);
	setColumnNumber(0);
	setLexeme(pLexeme);
}

/*** Set the current token's type ***/
void Token::setType(unsigned char pType)
{
	type = pType;
}

/*** Get the current token's type ***/
unsigned char Token::getType()
{
	return type;
}

/*** Set the current token's lexeme ***/
void Token::setLexeme(std::string pLexeme)
{
	lexeme = pLexeme;
}

/*** Get the current token's lexeme ***/
std::string Token::getLexeme()
{
	return lexeme;
}

/*** Set the current token's line number ***/
void Token::setLineNumber(unsigned long pLineNum)
{
	lineNum = pLineNum;
}

/*** Get the current token's line number ***/
unsigned long Token::getLineNumber()
{
	return lineNum;
}

/*** Set the current token's column number ***/
void Token::setColumnNumber(unsigned long pColNum)
{
	colNum = pColNum;
}

/*** Get the current token's column number ***/
unsigned long Token::getColumnNumber()
{
	return colNum;
}

/*** Destructor ***/
Token::~Token()
{
}
