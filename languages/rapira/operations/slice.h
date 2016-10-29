#ifndef SLICE_H
#define SLICE_H

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
#include "../exceptions/invalidindex.h"
#include "../exceptions/missingargument.h"

class Slice : public Object
{

	public:

		/*** Constructor ***/
		Slice();

		/*** Constructor ***/
		Slice(Object* pArg1, Object* pArg2, Object* pArg3);

		/*** Get this operation's type ***/
		unsigned char getType();

		/*** Set the first argument ***/
		void setArgument1(Object* pArg);

		/*** Set the second argument ***/
		void setArgument2(Object* pArg);

		/*** Set the third argument ***/
		void setArgument3(Object* pArg);

		/*** Get the first argument ***/
		Object* getArgument1();

		/*** Get the second argument ***/
		Object* getArgument2();

		/*** Get the third argument ***/
		Object* getArgument3();

		/*** Evaluate this object ***/
		Object* evaluate();

		/*** Clone this object ***/
		Slice* clone() const
		{
			Slice* nsn = new Slice();
			nsn->setLineNumber(getLineNumber());
			nsn->setColumnNumber(getColumnNumber());
			if(arg1 != 0)
				nsn->setArgument1(arg1->clone());
			if(arg2 != 0)
				nsn->setArgument2(arg2->clone());
			if(arg3 != 0)
				nsn->setArgument3(arg3->clone());
			return nsn;
		}

		/*** Destructor ***/
		~Slice();

	private:

		/*** The first argument ***/
		Object* arg1;

		/*** The second argument ***/
		Object* arg2;

		/*** The third argument ***/
		Object* arg3;

};

#endif
