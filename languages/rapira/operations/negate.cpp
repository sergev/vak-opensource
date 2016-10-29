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

#include "negate.h"

/*** Constructor ***/
Negate::Negate()
{
	setArgument(0);
}

/*** Constructor ***/
Negate::Negate(Object* pArg)
{
	setArgument(pArg);
}

/*** Get this operation's type ***/
unsigned char Negate::getType()
{
	return OP_NEGATE;
}

/*** Set the first argument ***/
void Negate::setArgument(Object* pArg)
{
	arg = pArg;
}

/*** Evaluate this object ***/
Object* Negate::evaluate()
{
	if(arg == 0)
		throw MissingArgumentException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, 1);

	std::auto_ptr<Object> obj(arg->evaluate());

	if(obj->getType() == OBJ_INTEGER)
	{
		Integer* cast = static_cast<Integer*>(obj.get());
		return new Integer(-cast->getValue());
	}

	if(obj->getType() == OBJ_REAL)
	{
		Real* cast = static_cast<Real*>(obj.get());
		return new Real(-cast->getValue());
	}

	throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_INTEGER | OBJ_REAL, obj->getType(), 1);
	return 0;
}

/*** Destructor ***/
Negate::~Negate()
{
	delete arg;
}
