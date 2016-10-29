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

#include "integer.h"

/*** Constructor ***/
Integer::Integer()
{
	setValue(0);
}

/*** Constructor ***/
Integer::Integer(Token tok)
{
	if(tok.getType() != T_UNSIGNEDINT)
		throw InvalidInitException(getLineNumber(), getColumnNumber(), OBJ_INTEGER);

	setValue(tok.getLexeme());
	setLineNumber(tok.getLineNumber());
	setColumnNumber(tok.getColumnNumber());
}

/*** Constructor ***/
Integer::Integer(long pValue)
{
	setValue(pValue);
}

/*** Constructor ***/
Integer::Integer(std::string pValue)
{
	setValue(pValue);
}

/*** Get this object's type ***/
unsigned char Integer::getType()
{
	return OBJ_INTEGER;
}

/*** Set this integer's value ***/
void Integer::setValue(long pValue)
{
	value = pValue;
}

/*** Set this integer's value ***/
void Integer::setValue(std::string pValue)
{
	std::string strNum = pValue;
	if(strNum.length() > 0 && strNum.at(0) == '+')
		strNum.erase(0, 1);
	std::istringstream(strNum) >> value;
}

/*** Get this integer's value ***/
long Integer::getValue()
{
	return value;
}

/*** Evaluate this object ***/
Object* Integer::evaluate()
{
	return clone();
}

/*** Destructor ***/
Integer::~Integer()
{
}
