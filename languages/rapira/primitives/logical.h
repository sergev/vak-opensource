#ifndef LOGICAL_H
#define LOGICAL_H

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

#include "../object.h"
#include "../token.h"
#include "../exceptions/invalidinit.h"

class Logical : public Object
{

	public:

		/*** Constructor ***/
		Logical();

		/*** Constructor ***/
		Logical(Token tok);

		/*** Constructor ***/
		Logical(bool pValue);

		/*** Get this object's type ***/
		unsigned char getType();

		/*** Set this logical's value ***/
		void setValue(bool pValue);

		/*** Get this logical's value ***/
		bool getValue();

		/*** Evaluate this object ***/
		Object* evaluate();

		/*** Clone this object ***/
		Logical* clone() const { return new Logical(value); }

		/*** Destructor ***/
		~Logical();

	private:

		/*** The value of this integer ***/
		bool value;

};

#endif
