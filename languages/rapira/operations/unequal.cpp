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

#include "unequal.h"

/*** Constructor ***/
Unequal::Unequal()
{
	setArgument1(0);
	setArgument2(0);
}

/*** Constructor ***/
Unequal::Unequal(Object* pArg1, Object* pArg2)
{
	setArgument1(pArg1);
	setArgument2(pArg2);
}

/*** Get this operation's type ***/
unsigned char Unequal::getType()
{
	return OP_UNEQUAL;
}

/*** Set the first argument ***/
void Unequal::setArgument1(Object* pArg)
{
	arg1 = pArg;
}

/*** Set the second argument ***/
void Unequal::setArgument2(Object* pArg)
{
	arg2 = pArg;
}

/*** Evaluate this object ***/
Object* Unequal::evaluate()
{
	std::auto_ptr<Object> eqResult;
	try
	{
		eqResult.reset(Equal(arg1->clone(), arg2->clone()).evaluate());
	}
	catch(InvalidTypeException& e)
	{
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), e.getExpectedType(), e.getReceivedType(), e.getArgumentNumber());
	}
	catch(MissingArgumentException& e)
	{
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), e.getExpectedType(), e.getArgumentNumber());
	}
	catch(Excep& e)
	{
		throw e;
	}
	Object* notResult = Not(eqResult.get()->clone()).evaluate();
	return notResult;
}

/*** Destructor ***/
Unequal::~Unequal()
{
	delete arg1;
	delete arg2;
}
