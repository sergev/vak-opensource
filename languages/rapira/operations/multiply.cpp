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

#include "multiply.h"

/*** Constructor ***/
Multiply::Multiply()
{
	setArgument1(0);
	setArgument2(0);
}

/*** Constructor ***/
Multiply::Multiply(Object* pArg1, Object* pArg2)
{
	setArgument1(pArg1);
	setArgument2(pArg2);
}

/*** Get this operation's type ***/
unsigned char Multiply::getType()
{
	return OP_MULTIPLY;
}

/*** Set the first argument ***/
void Multiply::setArgument1(Object* pArg)
{
	arg1 = pArg;
}

/*** Set the second argument ***/
void Multiply::setArgument2(Object* pArg)
{
	arg2 = pArg;
}

/*** Evaluate this object ***/
Object* Multiply::evaluate()
{
	if(arg1 == 0)
		throw MissingArgumentException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL | OBJ_TEXT | OBJ_SEQUENCE, 1);
	if(arg2 == 0)
		throw MissingArgumentException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL | OBJ_TEXT | OBJ_SEQUENCE, 2);

	std::auto_ptr<Object> obj1(arg1->evaluate());
	std::auto_ptr<Object> obj2(arg2->evaluate());

	if(obj1->getType() == OBJ_INTEGER)
	{
		Integer* cast1 = static_cast<Integer*>(obj1.get());

		if(obj2->getType() == OBJ_INTEGER)
		{
			Integer* cast2 = static_cast<Integer*>(obj2.get());
			return new Integer(cast1->getValue() * cast2->getValue());
		}
		
		if(obj2->getType() == OBJ_REAL)
		{
			Real* cast2 = static_cast<Real*>(obj2.get());
			return new Real(cast1->getValue() * cast2->getValue());
		}

		if(obj2->getType() == OBJ_TEXT)
		{
			Text* cast2 = static_cast<Text*>(obj2.get());

			if(cast1->getValue() <= 0)
				throw NegativeValueException(getLineNumber(), getColumnNumber(), cast1->getValue(), 1);

			Text* txtResult = new Text();

			for(unsigned int i = 0; i < (unsigned int) cast1->getValue(); i++)
				txtResult->setValue(txtResult->getValue() + cast2->getValue());

			return txtResult;
		}

		if(obj2->getType() == OBJ_SEQUENCE)
		{
			Sequence* cast2 = static_cast<Sequence*>(obj2.get());

			if(cast1->getValue() <= 0)
				throw NegativeValueException(getLineNumber(), getColumnNumber(), cast1->getValue(), 1);

			Sequence* seqResult = new Sequence();

			for(unsigned int i = 0; i < (unsigned int) cast1->getValue(); i++)
			{
				for(unsigned int j = 0; j < cast2->getLength(); j++)
					seqResult->pushObject(cast2->getObject(j)->clone());
			}

			return seqResult;
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL | OBJ_TEXT | OBJ_SEQUENCE, obj2->getType(), 2);
	}

	if(obj1->getType() == OBJ_REAL)
	{
		Real* cast1 = static_cast<Real*>(obj1.get());

		if(obj2->getType() == OBJ_INTEGER)
		{
			Integer* cast2 = static_cast<Integer*>(obj2.get());
			return new Real(cast1->getValue() * cast2->getValue());
		}

		if(obj2->getType() == OBJ_REAL)
		{
			Real* cast2 = static_cast<Real*>(obj2.get());
			return new Real(cast1->getValue() * cast2->getValue());
		}
		
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, obj2->getType(), 2);
	}

	if(obj1->getType() == OBJ_TEXT)
	{
		Text* cast1 = static_cast<Text*>(obj1.get());

		if(obj2->getType() != OBJ_INTEGER)
			throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER, obj2->getType(), 2);

		Integer* cast2 = static_cast<Integer*>(obj2.get());

		if(cast2->getValue() <= 0)
			throw NegativeValueException(getLineNumber(), getColumnNumber(), cast2->getValue(), 2);

		Text* txtResult = new Text();

		for(unsigned int i = 0; i < (unsigned int) cast2->getValue(); i++)
			txtResult->setValue(txtResult->getValue() + cast1->getValue());

		return txtResult;
	}

	if(obj1->getType() == OBJ_SEQUENCE)
	{
		if(obj2->getType() != OBJ_INTEGER)
			throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER, obj2->getType(), 2);

		Sequence* cast1 = static_cast<Sequence*>(obj1.get());
		Integer* cast2 = static_cast<Integer*>(obj2.get());

		if(cast2->getValue() <= 0)
			throw NegativeValueException(getLineNumber(), getColumnNumber(), cast2->getValue(), 2);

		Sequence* seqResult = new Sequence();

		for(unsigned int i = 0; i < (unsigned int) cast2->getValue(); i++)
		{
			for(unsigned int j = 0; j < cast1->getLength(); j++)
				seqResult->pushObject(cast1->getObject(j)->clone());
		}

		return seqResult;
	}

	throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL | OBJ_TEXT | OBJ_SEQUENCE, obj1->getType(), 1);
	return 0;
}

/*** Destructor ***/
Multiply::~Multiply()
{
	delete arg1;
	delete arg2;
}
