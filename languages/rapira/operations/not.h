#ifndef NOT_H
#define NOT_H

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

class Not : public Object
{

	public:

		/*** Constructor ***/
		Not();

		/*** Constructor ***/
		Not(Object* pArg);

		/*** Get this operation's type ***/
		unsigned char getType();

		/*** Set the first argument ***/
		void setArgument(Object* pArg);

		/*** Evaluate this object ***/
		Object* evaluate();

		/*** Clone this object ***/
		Not* clone() const
		{
			Not* cnn = new Not();
			cnn->setLineNumber(getLineNumber());
			cnn->setColumnNumber(getColumnNumber());
			if(arg != 0)
				cnn->setArgument(arg->clone());
			return cnn;
		}

		/*** Destructor ***/
		~Not();

	private:

		/*** The argument ***/
		Object* arg;

};

#endif
