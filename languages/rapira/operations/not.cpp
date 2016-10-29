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

#include "not.h"

/*** Constructor ***/
Not::Not()
{
	setArgument(0);
}

/*** Constructor ***/
Not::Not(Object* pArg)
{
	setArgument(pArg);
}

/*** Get this operation's type ***/
unsigned char Not::getType()
{
	return OP_NOT;
}

/*** Set the first argument ***/
void Not::setArgument(Object* pArg)
{
	arg = pArg;
}

/*** Evaluate this object ***/
Object* Not::evaluate()
{
	if(arg == 0)
		throw MissingArgumentException(getLineNumber(), getColumnNumber(), OBJ_LOGICAL, 1);

	std::auto_ptr<Object> obj(arg->evaluate());

	if(obj->getType() != OBJ_LOGICAL)
		throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_LOGICAL, obj->getType(), 1);

	Logical* cast = static_cast<Logical*>(obj.get());

	return new Logical(!cast->getValue());
}

/*** Destructor ***/
Not::~Not()
{
	delete arg;
}
