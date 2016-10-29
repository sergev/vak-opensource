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

#include "add.h"

/*** Constructor ***/
Add::Add()
{
	setArgument1(0);
	setArgument2(0);
}

/*** Constructor ***/
Add::Add(Object* pArg1, Object* pArg2)
{
	setArgument1(pArg1);
	setArgument2(pArg2);
}

/*** Get this operation's type ***/
unsigned char Add::getType()
{
	return OP_ADD;
}

/*** Set the first argument ***/
void Add::setArgument1(Object* pArg)
{
	arg1 = pArg;
}

/*** Set the second argument ***/
void Add::setArgument2(Object* pArg)
{
	arg2 = pArg;
}

/*** Evaluate this object ***/
Object* Add::evaluate()
{
	if(arg1 == 0)
		throw MissingArgumentException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL | OBJ_TEXT | OBJ_SEQUENCE, 1);
	if(arg2 == 0)
		throw MissingArgumentException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL | OBJ_TEXT | OBJ_SEQUENCE, 2);

	std::auto_ptr<Object> obj1(arg1->evaluate());
	std::auto_ptr<Object> obj2(arg2->evaluate());

	if(obj1->getType() == OBJ_EMPTY)
		return obj2->clone();

	if(obj2->getType() == OBJ_EMPTY)
		return obj1->clone();

	if(obj1->getType() == OBJ_INTEGER)
	{
		Integer* cast1 = static_cast<Integer*>(obj1.get());

		if(obj2->getType() == OBJ_INTEGER)
		{
			Integer* cast2 = static_cast<Integer*>(obj2.get());
			return new Integer(cast1->getValue() + cast2->getValue());
		}
		
		if(obj2->getType() == OBJ_REAL)
		{
			Real* cast2 = static_cast<Real*>(obj2.get());
			return new Real(cast1->getValue() + cast2->getValue());
		}
		
		if(obj2->getType() == OBJ_TEXT)
		{
			Text* cast2 = static_cast<Text*>(obj2.get());
			std::stringstream ss;
			ss << cast1->getValue();
			return new Text(ss.str() + cast2->getValue());
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL | OBJ_TEXT, obj2->getType(), 2);
	}

	if(obj1->getType() == OBJ_REAL)
	{
		Real* cast1 = static_cast<Real*>(obj1.get());

		if(obj2->getType() == OBJ_INTEGER)
		{
			Integer* cast2 = static_cast<Integer*>(obj2.get());
			return new Real(cast1->getValue() + cast2->getValue());
		}

		if(obj2->getType() == OBJ_REAL)
		{
			Real* cast2 = static_cast<Real*>(obj2.get());
			return new Real(cast1->getValue() + cast2->getValue());
		}

		if(obj2->getType() == OBJ_TEXT)
		{
			Text* cast2 = static_cast<Text*>(obj2.get());
			std::stringstream ss;
			ss << cast1->getValue();
			return new Text(ss.str() + cast2->getValue());
		}
		
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL | OBJ_TEXT, obj2->getType(), 2);
	}

	if(obj1->getType() == OBJ_TEXT)
	{
		Text* cast1 = static_cast<Text*>(obj1.get());

		if(obj2->getType() == OBJ_INTEGER)
		{
			Integer* cast2 = static_cast<Integer*>(obj2.get());
			std::stringstream ss;
			ss << cast2->getValue();
			return new Text(cast1->getValue() + ss.str());
		}

		if(obj2->getType() == OBJ_REAL)
		{
			Real* cast2 = static_cast<Real*>(obj2.get());
			std::stringstream ss;
			ss << cast2->getValue();
			return new Text(cast1->getValue() + ss.str());
		}

		if(obj2->getType() == OBJ_TEXT)
		{
			Text* cast2 = static_cast<Text*>(obj2.get());
			return new Text(cast1->getValue() + cast2->getValue());
		}
		
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL | OBJ_TEXT, obj2->getType(), 2);
	}

	if(obj1->getType() == OBJ_SEQUENCE)
	{
		if(obj2->getType() != OBJ_SEQUENCE)
			throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_SEQUENCE, obj2->getType(), 2);

		Sequence* cast1 = static_cast<Sequence*>(obj1.get());
		Sequence* cast2 = static_cast<Sequence*>(obj2.get());

		Sequence* seqResult = new Sequence(cast1->getList());
		for(unsigned int i = 0; i < cast2->getLength(); i++)
			seqResult->pushObject(cast2->getObject(i)->clone());

		return seqResult;
	}

	throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL | OBJ_TEXT | OBJ_SEQUENCE, obj2->getType(), 1);
	return 0;
}

/*** Destructor ***/
Add::~Add()
{
	delete arg1;
	delete arg2;
}
