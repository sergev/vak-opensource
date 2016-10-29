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

#include "select.h"

/*** Constructor ***/
Select::Select()
{
	setArgument1(0);
	setArgument2(0);
}

/*** Constructor ***/
Select::Select(Object* pArg1, Object* pArg2)
{
	setArgument1(pArg1);
	setArgument2(pArg2);
}

/*** Get this operation's type ***/
unsigned char Select::getType()
{
	return OP_SYMBOL;
}

/*** Set the first argument ***/
void Select::setArgument1(Object* pArg)
{
	arg1 = pArg;
}

/*** Set the second argument ***/
void Select::setArgument2(Object* pArg)
{
	arg2 = pArg;
}

/*** Get the first argument ***/
Object* Select::getArgument1()
{
	return arg1;
}

/*** Get the second argument ***/
Object* Select::getArgument2()
{
	return arg2;
}

/*** Evaluate this object ***/
Object* Select::evaluate()
{
	if(arg1 == 0)
		throw MissingArgumentException(getLineNumber(), getColumnNumber(), OBJ_TEXT | OBJ_SEQUENCE, 1);
	if(arg2 == 0)
		throw MissingArgumentException(getLineNumber(), getColumnNumber(), OBJ_INTEGER, 1);

	std::auto_ptr<Object> obj1(arg1->evaluate());
	std::auto_ptr<Object> obj2(arg2->evaluate());

	if(obj2->getType() != OBJ_INTEGER)
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER, obj2->getType(), 2);

	if(obj1->getType() == OBJ_TEXT)
	{
		Text* cast1	= static_cast<Text*>(obj1.get());
		Integer* cast2	= static_cast<Integer*>(obj2.get());

		if(cast2->getValue() < 1 || cast2->getValue() > (int) cast1->getLength())
			throw InvalidIndexException(getLineNumber(), getColumnNumber(), cast2->getValue(), 2);

		return cast1->getChar(cast2->getValue() - 1);
	}

	if(obj1->getType() == OBJ_SEQUENCE)
	{
		Sequence* cast1	= static_cast<Sequence*>(obj1.get());
		Integer* cast2	= static_cast<Integer*>(obj2.get());

		if(cast2->getValue() < 1 || cast2->getValue() > (int) cast1->getLength())
			throw InvalidIndexException(getLineNumber(), getColumnNumber(), cast2->getValue(), 2);

		return cast1->getObject(cast2->getValue() - 1)->clone();
	}

	throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_TEXT | OBJ_SEQUENCE, obj1->getType(), 1);
	return 0;
}

/*** Destructor ***/
Select::~Select()
{
	delete arg1;
	delete arg2;
}
