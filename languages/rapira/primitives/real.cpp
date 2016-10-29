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

#include "real.h"

/*** Constructor ***/
Real::Real()
{
	setValue(0.0);
}

/*** Constructor ***/
Real::Real(Token tok)
{
	if(tok.getType() != T_UNSIGNEDREAL)
		throw InvalidInitException(getLineNumber(), getColumnNumber(), OBJ_REAL);

	setValue(tok.getLexeme());
	setLineNumber(tok.getLineNumber());
	setColumnNumber(tok.getColumnNumber());
}

/*** Constructor ***/
Real::Real(double pValue)
{
	setValue(pValue);
}

/*** Constructor ***/
Real::Real(std::string pValue)
{
	setValue(pValue);
}

/*** Get this object's type ***/
unsigned char Real::getType()
{
	return OBJ_REAL;
}

/*** Set this real's value ***/
void Real::setValue(double pValue)
{
	value = pValue;
}

/*** Set this real's value ***/
void Real::setValue(std::string pValue)
{
	int index = pValue.find('e');

	if(index == (int) std::string::npos)
		std::istringstream(pValue) >> value;
	else
	{
		std::istringstream(pValue.substr(0, index)) >> value;
		int exponent;
		std::istringstream(pValue.substr(index + 1, pValue.length())) >> exponent;
		value *= pow(10.0, exponent);
	}
}

/*** Get this real's value ***/
double Real::getValue()
{
	return value;
}

/*** Evaluate this object ***/
Object* Real::evaluate()
{
	return clone();
}

/*** Destructor ***/
Real::~Real()
{
}
