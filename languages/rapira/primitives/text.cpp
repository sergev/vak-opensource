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

#include "text.h"

/*** Constructor ***/
Text::Text()
{
	setValue("");
}

/*** Constructor ***/
Text::Text(Token tok)
{
	if(tok.getType() != T_STRING)
		throw InvalidInitException(getLineNumber(), getColumnNumber(), OBJ_TEXT);

	setValue(tok.getLexeme());
	setLineNumber(tok.getLineNumber());
	setColumnNumber(tok.getColumnNumber());
}

/*** Constructor ***/
Text::Text(std::string pValue)
{
	setValue(pValue);
}

/*** Constructor ***/
Text::Text(char pValue)
{
	setValue(pValue);
}

/*** Get this object's type ***/
unsigned char Text::getType()
{
	return OBJ_TEXT;
}

/*** Get this text's length ***/
unsigned int Text::getLength()
{
	return value.length();
}

/*** Set this text's value ***/
void Text::setValue(std::string pValue)
{
	value = pValue;
}

/*** Set this text's value ***/
void Text::setValue(char pValue)
{
	value = std::string(1, pValue);
}

/*** Get this text's value ***/
std::string Text::getValue()
{
	return value;
}

/*** Evaluate this object ***/
Object* Text::evaluate()
{
	return clone();
}

/*** Get a character ***/
Text* Text::getChar(unsigned int index)
{
	return new Text(value.at(index));
}

/*** Destructor ***/
Text::~Text()
{
}
