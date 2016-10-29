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

#include "specialfunction.h"

/*** Constructor ***/
SpecialFunction::SpecialFunction(unsigned char pId)
{
	id = pId;
}

/*** Get this operation's type ***/
unsigned char SpecialFunction::getType()
{
	return OBJ_SPFUNCTION;
}

/*** Add an argument ***/
void SpecialFunction::addArgument(Object *pArg)
{
	args.push_back(pArg);
}

/*** Evaluate this object ***/
Object* SpecialFunction::evaluate()
{
	if(args.size() == 0)
		throw Excep(getLineNumber(), getColumnNumber(), "Expected argument to special function!");

	std::auto_ptr<Object> arg1(args.at(0)->evaluate());

	if(id == SPF_INDEX)
	{
		if(args.size() != 2)
			throw Excep(getLineNumber(), getColumnNumber(), "Invalid number of arguments passed to special function!");

		std::auto_ptr<Object> arg2(args.at(1)->evaluate());

		if(arg2->getType() == OBJ_TEXT)
		{
			if(arg1->getType() != OBJ_TEXT)
				throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_TEXT, arg1->getType(), 1);

			Text* cast1 = static_cast<Text*>(arg1.get());
			Text* cast2 = static_cast<Text*>(arg2.get());

			size_t index = cast2->getValue().find(cast1->getValue());

			if(index != std::string::npos)
				return new Integer(index + 1);

			return new Integer(0);
		}

		if(arg2->getType() == OBJ_SEQUENCE)
		{
				Sequence* cast = dynamic_cast<Sequence*>(arg2.get());

				for(unsigned int i = 0; i < cast->getLength(); i++)
				{
					std::auto_ptr<Logical> eqOp(static_cast<Logical*>(Equal(arg1->clone(), cast->getObject(i)->clone()).evaluate()));
					if(eqOp->getValue() == true)
						return new Integer(i + 1);
				}

				return new Integer(0);
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_TEXT | OBJ_SEQUENCE, arg2->getType(), 2);
	}

	if(args.size() > 1)
		throw Excep(getLineNumber(), getColumnNumber(), "Invalid number of arguments passed to special function!");

	if(id == SPF_ABS)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());
			if(cast->getValue() < 0)
				return new Integer(-cast->getValue());
			return new Integer(cast->getValue());
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());
			if(cast->getValue() < 0)
				return new Real(-cast->getValue());
			return new Real(cast->getValue());
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_SIGN)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());
			if(cast->getValue() < 0)
				return new Integer(-1);
			if(cast->getValue() > 0)
				return new Integer(1);
			return new Integer(0);
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());
			if(cast->getValue() < 0)
				return new Integer(-1);
			if(cast->getValue() > 0)
				return new Integer(1);
			return new Integer(0);
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_SQRT)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());

			if(cast->getValue() < 0)
				throw NegativeValueException(getLineNumber(), getColumnNumber(), cast->getValue(), 1);

			double res = sqrt((double) cast->getValue());
			if((long) res == res)
				return new Integer((long) res);
			return new Real(res);
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());

			if(cast->getValue() < 0)
				throw NegativeValueException(getLineNumber(), getColumnNumber(), cast->getValue(), 1);

			double res = sqrt((double) cast->getValue());
			return new Real(res);
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_ENTIER)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());
			return new Integer(cast->getValue());
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());
			return new Integer(floor(cast->getValue()));
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_ROUND)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());
			return new Integer(cast->getValue());
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());
			long rounded = cast->getValue() < 0.0 ? ceil(cast->getValue() - 0.5) : floor(cast->getValue() + 0.5);
			return new Integer(rounded);
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_RAND)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());

			if(cast->getValue() < 0)
				throw NegativeValueException(getLineNumber(), getColumnNumber(), cast->getValue(), 1);

			double f = (double) rand() / RAND_MAX;
			return new Real( f * cast->getValue() );
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());

			if(cast->getValue() < 0)
				throw NegativeValueException(getLineNumber(), getColumnNumber(), cast->getValue(), 1);

			double f = (double) rand() / RAND_MAX;
			return new Real( f * cast->getValue() );
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_INTRAND)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());
			if(cast->getValue() < 0)
				throw NegativeValueException(getLineNumber(), getColumnNumber(), cast->getValue(), 1);
			return new Integer( rand() % cast->getValue() + 1 );
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());
			if(cast->getValue() < 0)
				throw NegativeValueException(getLineNumber(), getColumnNumber(), cast->getValue(), 1);
			return new Integer( rand() % (long) cast->getValue() + 1 );
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_ISEMPTY)
	{
		return new Logical(arg1->getType() == OBJ_EMPTY);
	}

	if(id == SPF_ISLOG)
	{
		return new Logical(arg1->getType() == OBJ_LOGICAL);
	}

	if(id == SPF_ISINT)
	{
		return new Logical(arg1->getType() == OBJ_INTEGER);
	}

	if(id == SPF_ISREAL)
	{
		return new Logical(arg1->getType() == OBJ_REAL);
	}

	if(id == SPF_ISTEXT)
	{
		return new Logical(arg1->getType() == OBJ_TEXT);
	}

	if(id == SPF_ISSEQ)
	{
		return new Logical(arg1->getType() == OBJ_SEQUENCE);
	}

	if(id == SPF_ISPROC)
	{
		return new Logical(arg1->getType() == OBJ_PROCEDURE);
	}

	if(id == SPF_ISFUN)
	{
		return new Logical(arg1->getType() == OBJ_FUNCTION || arg1->getType() == OBJ_SPFUNCTION);
	}

	if(id == SPF_SIN)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());
			return new Real(sin((double) cast->getValue()));
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());
			return new Real(sin(cast->getValue()));
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_COS)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());
			return new Real(cos((double) cast->getValue()));
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());
			return new Real(cos(cast->getValue()));
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_TG)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());
			return new Real(tan((double) cast->getValue()));
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());
			return new Real(tan(cast->getValue()));
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_ARCSIN)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());
			if(errno == EDOM)
				throw new Excep(getLineNumber(), getColumnNumber(), "Invalid value passed to special function!");
			return new Real(exp((double) cast->getValue()));
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());
			if(errno == EDOM)
				throw new Excep(getLineNumber(), getColumnNumber(), "Invalid value passed to special function!");
			return new Real(exp(cast->getValue()));
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_ARCTG)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());
			if(errno == EDOM)
				throw new Excep(getLineNumber(), getColumnNumber(), "Invalid value passed to special function!");
			return new Real(exp((double) cast->getValue()));
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());
			if(errno == EDOM)
				throw new Excep(getLineNumber(), getColumnNumber(), "Invalid value passed to special function!");
			return new Real(exp(cast->getValue()));
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_EXP)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());
			return new Real(exp((double) cast->getValue()));
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());
			return new Real(exp(cast->getValue()));
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_LN)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());
			return new Real(log((double) cast->getValue()));
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());
			return new Real(log(cast->getValue()));
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	if(id == SPF_LG)
	{
		if(arg1->getType() == OBJ_INTEGER)
		{
			Integer* cast = static_cast<Integer*>(arg1.get());
			return new Real(log10((double) cast->getValue()));
		}

		if(arg1->getType() == OBJ_REAL)
		{
			Real* cast = static_cast<Real*>(arg1.get());
			return new Real(log10(cast->getValue()));
		}

		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, arg1->getType(), 1);
	}

	throw Excep(getLineNumber(), getColumnNumber(), "Invalid special function!");
}

/*** Destructor ***/
SpecialFunction::~SpecialFunction()
{
	// Loop through the arguments
	for(unsigned int i = 0; i < args.size(); i++)
		delete args.at(i);
}
