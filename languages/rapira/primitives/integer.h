#ifndef INTEGER_H
#define INTEGER_H

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

#include <sstream>
#include "../object.h"
#include "../token.h"
#include "../exceptions/invalidinit.h"

class Integer : public Object
{

	public:

		/*** Constructor ***/
		Integer();

		/*** Constructor ***/
		Integer(Token tok);

		/*** Constructor ***/
		Integer(long pValue);

		/*** Constructor ***/
		Integer(std::string pValue);

		/*** Get this object's type ***/
		unsigned char getType();

		/*** Set this integer's value ***/
		void setValue(long pValue);

		/*** Set this integer's value ***/
		void setValue(std::string pValue);

		/*** Get this integer's value ***/
		long getValue();

		/*** Evaluate this object ***/
		Object* evaluate();

		/*** Clone this object ***/
		Integer* clone() const { return new Integer(value); }

		/*** Destructor ***/
		~Integer();

	private:

		/*** The value of this integer ***/
		long value;

};

#endif
