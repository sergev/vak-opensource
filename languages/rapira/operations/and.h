#ifndef AND_H
#define AND_H

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

#include "../primitives/logical.h"
#include "../exceptions/invalidtype.h"
#include "../exceptions/missingargument.h"

class And : public Object
{

	public:

		/*** Constructor ***/
		And();

		/*** Constructor ***/
		And(Object* pArg1, Object* pArg2);

		/*** Get this operation's type ***/
		unsigned char getType();

		/*** Set the first argument ***/
		void setArgument1(Object* pArg);

		/*** Set the second argument ***/
		void setArgument2(Object* pArg);

		/*** Evaluate this object ***/
		Object* evaluate();

		/*** Clone this object ***/
		And* clone() const
		{
			And* can = new And();
			can->setLineNumber(getLineNumber());
			can->setColumnNumber(getColumnNumber());
			if(arg1 != 0)
				can->setArgument1(arg1->clone());
			if(arg2 != 0)
				can->setArgument2(arg2->clone());
			return can;
		}

		/*** Destructor ***/
		~And();

	private:

		/*** The first argument ***/
		Object* arg1;

		/*** The second argument ***/
		Object* arg2;

};

#endif

