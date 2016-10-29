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

#include "and.h"

/*** Constructor ***/
And::And()
{
	setArgument1(0);
	setArgument2(0);
}

/*** Constructor ***/
And::And(Object* pArg1, Object* pArg2)
{
	setArgument1(pArg1);
	setArgument2(pArg2);
}

/*** Get this operation's type ***/
unsigned char And::getType()
{
	return OP_AND;
}

/*** Set the first argument ***/
void And::setArgument1(Object* pArg)
{
	arg1 = pArg;
}

/*** Set the second argument ***/
void And::setArgument2(Object* pArg)
{
	arg2 = pArg;
}

/*** Evaluate this object ***/
Object* And::evaluate()
{
	if(arg1 == 0)
		throw MissingArgumentException(getLineNumber(), getColumnNumber(), OBJ_LOGICAL, 1);
	if(arg2 == 0)
		throw MissingArgumentException(getLineNumber(), getColumnNumber(), OBJ_LOGICAL, 2);

	std::auto_ptr<Object> obj1(arg1->evaluate());
	if(obj1->getType() != OBJ_LOGICAL)
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_LOGICAL, obj1->getType(), 1);

	std::auto_ptr<Object> obj2(arg2->evaluate());
	if(obj2->getType() != OBJ_LOGICAL)
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_LOGICAL, obj2->getType(), 2);

	Logical* cast1 = static_cast<Logical*>(obj1.get());
	Logical* cast2 = static_cast<Logical*>(obj2.get());

	return new Logical(cast1->getValue() && cast2->getValue());
}

/*** Destructor ***/
And::~And()
{
	delete arg1;
	delete arg2;
}
