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

#include "lesseq.h"

/*** Constructor ***/
LessEq::LessEq()
{
	setArgument1(0);
	setArgument2(0);
}

/*** Constructor ***/
LessEq::LessEq(Object* pArg1, Object* pArg2)
{
	setArgument1(pArg1);
	setArgument2(pArg2);
}

/*** Get this operation's type ***/
unsigned char LessEq::getType()
{
	return OP_LESSEQ;
}

/*** Set the first argument ***/
void LessEq::setArgument1(Object* pArg)
{
	arg1 = pArg;
}

/*** Set the second argument ***/
void LessEq::setArgument2(Object* pArg)
{
	arg2 = pArg;
}

/*** Evaluate this object ***/
Object* LessEq::evaluate()
{
	std::auto_ptr<Object> greatOperation;
	try
	{
		greatOperation.reset(Greater(arg1->clone(), arg2->clone()).evaluate());
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
	Object* result = Not(greatOperation->clone()).evaluate();
	return result;
}

/*** Destructor ***/
LessEq::~LessEq()
{
	delete arg1;
	delete arg2;
}
