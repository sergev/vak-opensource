#ifndef LENGTH_H
#define LENGTH_H

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

#include "../primitives/integer.h"
#include "../primitives/text.h"
#include "../primitives/sequence.h"
#include "../exceptions/invalidtype.h"
#include "../exceptions/missingargument.h"

class Length : public Object
{

	public:

		/*** Constructor ***/
		Length();

		/*** Constructor ***/
		Length(Object* pArg);

		/*** Get this operation's type ***/
		unsigned char getType();

		/*** Set the first argument ***/
		void setArgument(Object* pArg);

		/*** Evaluate this object ***/
		Object* evaluate();

		/*** Clone this object ***/
		Length* clone() const
		{
			Length* nln = new Length();
			nln->setLineNumber(getLineNumber());
			nln->setColumnNumber(getColumnNumber());
			if(arg != 0)
				nln->setArgument(arg->clone());
			return nln;
		}

		/*** Destructor ***/
		~Length();

	private:

		/*** The argument ***/
		Object* arg;

};

#endif
