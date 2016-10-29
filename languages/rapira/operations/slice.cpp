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

#include "slice.h"

/*** Constructor ***/
Slice::Slice()
{
	setArgument1(0);
	setArgument2(0);
	setArgument3(0);
}

/*** Constructor ***/
Slice::Slice(Object* pArg1, Object* pArg2, Object* pArg3)
{
	setArgument1(pArg1);
	setArgument2(pArg2);
	setArgument3(pArg3);
}

/*** Get this operation's type ***/
unsigned char Slice::getType()
{
	return OP_SLICE;
}

/*** Set the first argument ***/
void Slice::setArgument1(Object* pArg)
{
	arg1 = pArg;
}

/*** Set the second argument ***/
void Slice::setArgument2(Object* pArg)
{
	arg2 = pArg;
}

/*** Set the third argument ***/
void Slice::setArgument3(Object* pArg)
{
	arg3 = pArg;
}

/*** Get the first argument ***/
Object* Slice::getArgument1()
{
	return arg1;
}

/*** Get the second argument ***/
Object* Slice::getArgument2()
{
	return arg2;
}

/*** Get the third argument ***/
Object* Slice::getArgument3()
{
	return arg3;
}

/*** Evaluate this object ***/
Object* Slice::evaluate()
{
	if(arg1 == 0)
		throw MissingArgumentException(getLineNumber(), getColumnNumber(), OBJ_TEXT | OBJ_SEQUENCE, 1);

	std::auto_ptr<Object> obj1(arg1->evaluate());
	std::auto_ptr<Object> obj2;
	std::auto_ptr<Object> obj3;

	if(arg2 != 0)
		obj2.reset(arg2->evaluate());
	if(arg3 != 0)
		obj3.reset(arg3->evaluate());

	long index1 = 0;
	long index2 = 0;

	if(obj2.get() == 0)
		index1 = 1;
	else if(obj2->getType() == OBJ_INTEGER)
		index1 = static_cast<Integer*>(obj2.get())->getValue();
	else
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER, obj2->getType(), 2);

	if(obj1->getType() == OBJ_TEXT)
	{
		Text* cast1 = static_cast<Text*>(obj1.get());

		if(obj3.get() == 0)
			index2 = cast1->getLength();
		else if(obj3->getType() == OBJ_INTEGER)
			index2 = static_cast<Integer*>(obj3.get())->getValue();
		else
			throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER, obj3->getType(), 3);

		if(index1 < 1)
			throw InvalidIndexException(getLineNumber(), getColumnNumber(), index1, 1);
		if(index2 > (int) cast1->getLength())
			throw InvalidIndexException(getLineNumber(), getColumnNumber(), index2, 2);

		if(index1 > index2)
			return new Text();
		return new Text(cast1->getValue().substr(index1 - 1, index2 - index1 + 1));
	}

	if(obj1->getType() == OBJ_SEQUENCE)
	{
		Sequence* cast1 = static_cast<Sequence*>(obj1.get());

		if(obj3.get() == 0)
			index2 = cast1->getLength();
		else if(obj3->getType() == OBJ_INTEGER)
			index2 = static_cast<Integer*>(obj3.get())->getValue();
		else
			throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER, obj3->getType(), 3);

		if(index1 < 1)
			throw InvalidIndexException(getLineNumber(), getColumnNumber(), index1, 1);
		if(index2 > (int) cast1->getLength())
			throw InvalidIndexException(getLineNumber(), getColumnNumber(), index2, 2);

		Sequence* seqResult = new Sequence();

		if(index1 <= index2)
		{
			for(unsigned int i = index1 - 1; i < (unsigned) index2; i++)
				seqResult->pushObject(cast1->getObject(i)->clone());
		}

		return seqResult;
	}

	throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_TEXT | OBJ_SEQUENCE, obj1->getType(), 1);
	return 0;
}

/*** Destructor ***/
Slice::~Slice()
{
	delete arg1;
	delete arg2;
	delete arg3;
}
