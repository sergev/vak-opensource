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

#include "length.h"

/*** Constructor ***/
Length::Length()
{
	setArgument(0);
}

/*** Constructor ***/
Length::Length(Object* pArg)
{
	setArgument(pArg);
}

/*** Get this operation's type ***/
unsigned char Length::getType()
{
	return OP_LENGTH;
}

/*** Set the first argument ***/
void Length::setArgument(Object* pArg)
{
	arg = pArg;
}

/*** Evaluate this object ***/
Object* Length::evaluate()
{
	if(arg == 0)
		throw MissingArgumentException(getLineNumber(), getColumnNumber(), OBJ_TEXT | OBJ_SEQUENCE, 1);

	std::auto_ptr<Object> obj(arg->evaluate());

	if(obj->getType() == OBJ_EMPTY)
		return new Integer(0);

	if(obj->getType() == OBJ_TEXT)
	{
		Text* cast = static_cast<Text*>(obj.get());
		return new Integer(cast->getLength());
	}

	if(obj->getType() == OBJ_SEQUENCE)
	{
		Sequence* cast = static_cast<Sequence*>(obj.get());
		return new Integer(cast->getLength());
	}

	throw InvalidTypeException(getLineNumber(), getColumnNumber(), OBJ_TEXT | OBJ_SEQUENCE, obj->getType(), 1);
	return 0;
}

/*** Destructor ***/
Length::~Length()
{
	delete arg;
}
