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

#include "logical.h"

/*** Constructor ***/
Logical::Logical()
{
	setValue(false);
}

/*** Constructor ***/
Logical::Logical(Token tok)
{
	if(tok.getType() == T_YES)
		setValue(true);
	else if(tok.getType() == T_NO)
		setValue(false);
	else
		throw InvalidInitException(getLineNumber(), getColumnNumber(), OBJ_LOGICAL);

	setLineNumber(tok.getLineNumber());
	setColumnNumber(tok.getColumnNumber());
}

/*** Constructor ***/
Logical::Logical(bool pValue)
{
	setValue(pValue);
}

/*** Get this object's type ***/
unsigned char Logical::getType()
{
	return OBJ_LOGICAL;
}

/*** Set this logical's value ***/
void Logical::setValue(bool pValue)
{
	value = pValue;
}

/*** Get this logical's value ***/
bool Logical::getValue()
{
	return value;
}

/*** Evaluate this object ***/
Object* Logical::evaluate()
{
	return clone();
}

/*** Destructor ***/
Logical::~Logical()
{
}
